#define _WINSOCKAPI_
#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "autogen/config.h"
#include "astateful/algorithm/Generate/Recurse.hpp"
#include "astateful/async/net/client/Engine.hpp"
#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/server/Engine.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/plugin/Store/File.hpp"
#include "astateful/plugin/Store/Cache.hpp"
#include "astateful/plugin/Store/Mongo.hpp"
#include "astateful/plugin/Pipe/Scrypt.hpp"
#include "astateful/plugin/hmac.hpp"
#include "astateful/plugin/Compress.hpp"
#include "astateful/plugin/Expand.hpp"
#include "astateful/plugin/Key.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/crypto/md5.hpp"
#include "astateful/cache/Context.hpp"

#include <mutex>
#include <fstream>

using namespace astateful;
using namespace astateful::async;
using namespace astateful::plugin;

int main( int argc, char* const argv[] ) {
  std::mutex mutex;
  std::unique_lock<std::mutex> lock( mutex, std::defer_lock );

  pipe::server::callback_t callback;
  callback["log"] = [&]( const std::vector<uint8_t>& buffer ) {
    lock.lock();
    //std::string data( buffer.begin()+1, buffer.end() );
    lock.unlock();
  };

  pipe::server::Engine pipe( callback );
  pipe.start();

  int result = Catch::Session().run( argc, argv );

  pipe.kill();

  return result;
}

TEST_CASE( "pipe & store" ) {
  async::pipe::client::Engine pipe_client;
  pipe_client.start();

  SECTION( "cache" ) {
    const std::wstring address = L"localhost:11211";

    net::client::connection_t client_conn;
    client_conn[address] = [&]( net::client::Response * response,
                                const net::client::Request& request ) {
      return new net::client::Connection( response, request );
    };

    net::client::Engine net_client( pipe_client, client_conn );
    REQUIRE( net_client.runIPv4() );

    cache::Context cache( net_client, address );
    StoreCache store( cache );
    Pipe<bson::Serialize> pipe( store );
    const plugin::Key<std::string> key{ "prefix", "value", "hash" };

    SECTION( "non empty value" ) {
      auto input = std::make_unique<bson::Serialize>();
      input->append( bson::ElementString( "key", "value" ) );
      REQUIRE( pipe.Write( key, input ) );

      auto output = pipe.Read( key );
      REQUIRE( std::string( (*output)["key"] ) == "value" );
    }

    SECTION( "empty value" ) {
      REQUIRE( !pipe.Write( key, nullptr ) );
    }
  }

  SECTION( "file" ) {
    StoreFile store( L"." );
  }

  SECTION( "mongo" ) {
    const std::wstring address = L"localhost:27017";

    net::client::connection_t client_conn;
    client_conn[address] = [&]( net::client::Response * response,
                                const net::client::Request& request ) {
      return new net::client::Connection( response, request );
    };

    net::client::Engine net_client( pipe_client, client_conn );
    REQUIRE( net_client.runIPv4() );

    auto error = bson::error_e::CLEAN;

    mongo::Context mongo( net_client, address, error );
    StoreMongo store( mongo, "test.memoization" );
    Pipe<bson::Serialize> pipe( store );
    const plugin::Key<std::string> key{ "prefix", "value", "hash" };

    SECTION( "non empty" ) {
      auto input = std::make_unique<bson::Serialize>();
      input->append( bson::ElementString( "key", "value" ) );
      REQUIRE( pipe.Write( key, input ) );

      auto output = pipe.Read( key );
      REQUIRE( std::string( (*output)["key"] ) == "value" );
    }

    SECTION( "empty" ) {
      REQUIRE( !pipe.Write( key, nullptr ) );
    }
  }
}

TEST_CASE( "hmac" ) {
  SECTION( "normal case" ) {
    const std::string secret( "secret key" );
    const std::string payload( "this a test message" );

    auto hash = hmac( secret, { payload } );

    REQUIRE( hash == "3c465048bfe73acc8a0062fc44a41891b50c29906fab4d8ddbdd8e3615b210a3" );
  }
}

TEST_CASE( "key" ) {
  Key<std::string> key( "prefix", "value", "plaintext" );
  REQUIRE( key.prefix == "prefix");
  REQUIRE( key.value == "value" );
  REQUIRE( key.hash == crypto::md5( "plaintext" ) );
}

template<typename U, typename V> struct test_state :
  public algorithm::State<U, V> {
    test_state() = default;

    std::unique_ptr<U> operator()( const algorithm::value_t<U, V>&,
                                   const V& ) const override {
      return nullptr;
    }
};

template<typename U> void test_ce() {
  algorithm::Context<U, std::string> ic;
  algorithm::Context<U, std::string> oc;
  Compress<U, std::string> compress;
  Expand<U, std::string> expand;
    
  ic.value["one"] = algorithm::Value<U, std::string>("a", 1, std::make_unique<U>());
  ic.value["two"] = algorithm::Value<U, std::string>("b", 2, std::make_unique<U>());
  ic.value["three"] = algorithm::Value<U, std::string>("c", 0, std::make_unique<U>());
  ic.state["a"] = std::make_unique<test_state<U, std::string>>();
  ic.state["b"] = std::make_unique<test_state<U, std::string>>();
  ic.state["c"] = std::make_unique<test_state<U, std::string>>();

  oc.state["a"] = std::make_unique<test_state<U, std::string>>();
  oc.state["b"] = std::make_unique<test_state<U, std::string>>();
  oc.state["c"] = std::make_unique<test_state<U, std::string>>();

  REQUIRE( expand( oc, compress( ic ) ) );

  auto it = oc.value.begin();
  REQUIRE( it->first == "three" );
  REQUIRE( it->second.create() == "c" );
  REQUIRE( it->second.order() == 0 ); ++it;
  REQUIRE( it->first == "one" );
  REQUIRE( it->second.create() == "a" );
  REQUIRE( it->second.order() == 1 ); ++it;
  REQUIRE( it->first == "two" );
  REQUIRE( it->second.create() == "b" );
  REQUIRE( it->second.order() == 2 ); ++it;
}

TEST_CASE( "compress & expand" ) {
  test_ce<bson::Serialize>();
}

TEST_CASE( "iterate ") {

}

TEST_CASE( "mime" ) {

}

TEST_CASE( "node") {

}