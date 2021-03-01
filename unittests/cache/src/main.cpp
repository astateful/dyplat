#define _WINSOCKAPI_
#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "astateful/cache/Context.hpp"
#include "astateful/async/net/client/Engine.hpp"
#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/server/Engine.hpp"

#include <mutex>

using namespace astateful;
using namespace astateful::async;
using namespace astateful::cache;

void exec_request( std::string key,
                   std::string data,
                   const Context& context ) {
  REQUIRE( flush( context ) );
  REQUIRE( add( context, key, data ) == result_e::OK );
}

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

TEST_CASE( "cache" ) {
  const std::wstring address = L"localhost:11211";

  net::client::connection_t connection;
  connection[address] = [&]( net::client::Response * response,
                             const net::client::Request& request ) {
    return new net::client::Connection( response, request );
  };

  pipe::client::Engine pipe;
  pipe.start();

  net::client::Engine engine( pipe, connection );
  Context context( engine, address );

  REQUIRE( engine.runIPv4() );

  SECTION( "get" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" ) == result_e::OK );
    REQUIRE( get( context, "key" ) == "data" );
  }

  SECTION( "gets" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" ) == result_e::OK );

    uint64_t cas;
    REQUIRE( gets( context, "key", cas ) == "data" );
    REQUIRE( cas != 0 );
  }

  SECTION( "set" ) {
    REQUIRE( flush( context ) );
    REQUIRE( set( context, "key", "data" ) == result_e::OK );
    REQUIRE( set( context, "key", "data" ) == result_e::OK );
  }

  SECTION( "add" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" ) == result_e::OK );
    REQUIRE( add( context, "key", "data" ) == result_e::NOT_STORED );
  }

  SECTION( "replace" ) {
    REQUIRE( flush( context ) );
    REQUIRE( replace( context, "key", "data" ) == result_e::NOT_STORED );
    REQUIRE( set( context, "key", "data" ) == result_e::OK );
    REQUIRE( replace( context, "key", "replace" )  == result_e::OK );
    REQUIRE( get( context, "key" ) == "replace" );
  }

  SECTION( "append" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" )  == result_e::OK );
    REQUIRE( append( context, "key", "append" ) == result_e::OK );
    REQUIRE( get( context, "key" ) == "dataappend" );
    REQUIRE( append( context, "keyne", "datane" )  == result_e::NOT_STORED );
  }

  SECTION( "prepend" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" ) == result_e::OK );
    REQUIRE( prepend( context, "key", "prepend" ) == result_e::OK );
    REQUIRE( get( context, "key" ) == "prependdata" );
    REQUIRE( prepend( context, "keyne", "datane" ) == result_e::NOT_STORED );
  }

  SECTION( "cas" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" ) == result_e::OK );

    uint64_t cas_value;
    REQUIRE( gets( context, "key", cas_value ) == "data" );
    REQUIRE( cas_value != 0 );

    REQUIRE( cas( context, "key", "datan", cas_value ) == result_e::OK );
    REQUIRE( set( context, "key", "datann" ) == result_e::OK );
    REQUIRE( cas( context, "key", "datann", cas_value ) == result_e::NOT_STORED );
  }

  SECTION( "delete" ) {
    REQUIRE( flush( context ) );
    REQUIRE( add( context, "key", "data" ) == result_e::OK );
    REQUIRE( del( context, "key" ) == result_e::OK );
    REQUIRE( del( context, "key" ) == result_e::NOT_FOUND );
  }

  SECTION( "increment" ) {
    REQUIRE( flush( context ) );
    REQUIRE( inc( context, "key", 1 ) == "" );
    REQUIRE( add( context, "key", "999" ) == result_e::OK );
    REQUIRE( inc( context, "key", 1 ) == "1000" );

    REQUIRE( inc( context, "key", 100 ) == "1100" );
    REQUIRE( inc( context, "key", 50, false ) == "" );
    REQUIRE( inc( context, "key", 25 ) == "1175" );
    REQUIRE( inc( context, "key", 5 ) == "1180" );
  }

  SECTION( "decrement" ) {
    REQUIRE( flush( context ) );
    REQUIRE( dec( context, "key", 1 ) == "" );
    REQUIRE( add( context, "key", "999" ) == result_e::OK );
    REQUIRE( dec( context, "key", 1 ) == "998" );

    REQUIRE( dec( context, "key", 100 ) == "898" );
    REQUIRE( dec( context, "key", 50, false ) == "" );
    REQUIRE( dec( context, "key", 25 ) == "823" );
    REQUIRE( dec( context, "key", 10 ) == "813" );
    REQUIRE( dec( context, "key", 5 ) == "808" );
    REQUIRE( dec( context, "key", 900 ) == "0" );
  }

  SECTION( "mulithreaded set" ) {
    REQUIRE( flush( context ) );

    std::vector<std::thread> client( 20 );

    for ( size_t i = 0; i < client.size(); i++ ) {
      const std::string key = "key" + std::to_string( i );
      const std::string data = "data";

      client[i] = std::thread( exec_request,
                               key,
                               data,
                               std::ref( context ) );
    }

    for ( auto& thread : client ) thread.join();
  }
}
