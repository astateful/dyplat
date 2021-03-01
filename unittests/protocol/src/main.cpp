#define _WINSOCKAPI_
#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "autogen/config.h"
#include "astateful/algorithm/Generate/Recurse.hpp"
#include "astateful/algorithm/Parse.hpp"
#include "astateful/async/net/client/Engine.hpp"
#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/server/Engine.hpp"
#include "astateful/bson/JSON.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/plugin/Store/Cache.hpp"
#include "astateful/plugin/Store/Mongo.hpp"
#include "astateful/plugin/Pipe/Scrypt.hpp"
#include "astateful/plugin/Compress.hpp"
#include "astateful/plugin/Expand.hpp"
#include "astateful/plugin/Key.hpp"
#include "astateful/cache/Context.hpp"
#include "astateful/script/Init.hpp"
#include "astateful/protocol/Context.hpp"
#include "astateful/mongo/Context.hpp"

#include <mutex>
#include <fstream>

using namespace astateful;
using namespace astateful::async;
using namespace astateful::plugin;
using namespace astateful::protocol;

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

template<typename U, typename V> void run() {
  const std::wstring log_file_path( install_prefix() + L"/log.txt" );
  const std::wstring memoize_path( install_prefix() + L"/memoize" );
  const std::wstring public_html( install_prefix() + L"/public_html" );
  const std::wstring script_path( install_prefix() + L"/script" );
  const std::wstring memoize( L"memoize" );

  pipe::client::Engine pipe_client;
  pipe_client.start();

  net::client::connection_t client_conn;
  client_conn[L"localhost:11211"] = [&]( net::client::Response * response,
                                         const net::client::Request& request ) {
    return new net::client::Connection( response, request );
  };

  client_conn[L"localhost:27017"] = [&]( net::client::Response * response,
                                         const net::client::Request& request ) {
    return new net::client::Connection( response, request );
  };

  net::client::Engine net_client( pipe_client, client_conn );
  net_client.runIPv4();

  auto error = bson::error_e::CLEAN;

  mongo::Context mongo( net_client, L"localhost:27017", error );
  cache::Context cache( net_client, L"localhost:11211" );

  const StoreCache store( cache );
  const PipeScrypt<U> pipe( "password", store );
  const algorithm::Persist<U, V> persist;
  const algorithm::Parse<U, V> parse( persist );
  const algorithm::GenerateRecurse<U, V> generate( persist );

  script::Init script_init;

  Context<U,V> protocol( memoize_path,
                         public_html,
                         script_path,
                         mongo,
                         parse,
                         generate,
                         pipe );

  std::unique_ptr<Response<U, V>> response( protocol.generateResponse( "h1" ) );
  std::unique_ptr<Request<U, V>> request( protocol.generateRequest( "h1" ) );

  std::vector<uint8_t> output;

  SECTION( "ws upgrade" ) {
    std::string input = "GET /chat HTTP/1.1\r\n";
    input += "Host: echo.example.com\r\n";
    input += "Upgrade: websocket\r\n";
    input += "Connection: Upgrade\r\n";
    input += "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n";
    input += "Sec-WebSocket-Protocol: chat, superchat\r\n";
    input += "Sec-WebSocket-Version: 13\r\n";
    input += "Origin: http://echo.example.com\r\n\r\n";

    request->update( input );
    response->create( *request, pipe_client, output );

    std::string check = "HTTP/1.1 101 Switching Protocols\r\n";
    check += "Upgrade: websocket\r\n";
    check += "Connection: Upgrade\r\n";
    check += "Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=\r\n\r\n";

    REQUIRE( std::string( output.begin(), output.end() ) == check );

    response->clear();
    request->clear();
    output.clear();
  }

  SECTION( "script" ) {
    std::string input = "GET / HTTP/1.1\r\n";
    input += "Host: script.example.com\r\n";

    request->update( input );
    response->create( *request, pipe_client, output );

    std::string check = "HTTP/1.1 200 OK\r\n";
    check += "Content-Length: 9\r\n";
    check += "Content-Type: text/html; charset=UTF-8\r\n";
    check += "Connection: keep-alive\r\n\r\n";
    check += "55.000000";

    REQUIRE( std::string( output.begin(), output.end() ) == check );

    response->clear();
    request->clear();
    output.clear();
  }

  SECTION( "auth & graph" ) {
    bson::Serialize query;
    query.append( bson::ElementString( "username", "test_user" ) );
    query.append( bson::ElementString( "password", "test_password" ) );

    bson::Serialize result;
    if ( mongo::find_one( mongo, "astateful.user", query, result, error ) ) {
      REQUIRE( mongo::remove( mongo, "astateful.user", query, error ) );
    }

    REQUIRE( mongo::insert( mongo, "astateful.user", query, error ) );

    std::string input = "POST /obtain HTTP/1.1\r\n";
    input += "Host: token.example.com\r\n";
    input += "Content-Length: 41\r\n\r\n";
    input += "username=test_user&password=test_password";

    request->update( input );
    response->create( *request, pipe_client, output );

    std::string check = "HTTP/1.1 200 OK\r\n";
    check += "Content-Length: 197\r\n";
    check += "Content-Type: application/json;charset=UTF-8\r\n";
    check += "Cache-Control: no-store\r\n";
    check += "Pragma: no-cache\r\n";
    check += "Connection: keep-alive\r\n\r\n";

    std::string json_token( output.begin() + check.length(), output.end() );

    auto token = bson::json::convert( json_token, error );
    REQUIRE( error == bson::error_e::CLEAN );
    REQUIRE( bson::element_e( token["access_token"] ) == bson::element_e::STRING );

    response->clear();
    request->clear();
    output.clear();

    SECTION( "graph document operations" ) {
      bson::Serialize query;
      query.append( bson::ElementString( "username", "test_user" ) );
      query.append( bson::ElementString( "password", "test_password" ) );

      bson::Serialize result;
      REQUIRE( mongo::find_one( mongo, "astateful.user", query, result, error ) );

      SECTION( "get document" ) {
        std::string input = "GET /" + std::string( result["_id"] );
        input += "?collection=user&token=" + std::string( token["access_token"] );
        input += " HTTP/1.1\r\n";
        input += "Host: graph.astateful.com\r\n\r\n";

        request->update( input );
        response->create( *request, pipe_client, output );

        std::string check = "HTTP/1.1 200 OK\r\n";
        check += "Content-Length: 148\r\n";
        check += "Content-Type: application/json;charset=UTF-8\r\n";
        check += "Connection: keep-alive\r\n\r\n";

        std::string json_response( output.begin() + check.length(), output.end() );

        auto bson_response = bson::json::convert( json_response, error );
        REQUIRE( error == bson::error_e::CLEAN );

        response->clear();
        request->clear();
        output.clear();
      }
      
      SECTION( "post to connection" ) {
        for ( int pid = 0; pid < 5; ++pid ) {
          std::string input = "POST /" + std::string( result["_id"] ) + "/posts";
          input += "?collection=user&token=" + std::string( token["access_token"] );
          input += " HTTP/1.1\r\n";
          input += "Host: graph.astateful.com\r\n";
          input += "Content-Length: 18\r\n\r\n";
          input += "id=" + std::to_string( pid ) + "&body=new_body";

          request->update( input );
          response->create( *request, pipe_client, output );

          response->clear();
          request->clear();
          output.clear();
        }
      }
    }
  }
}

TEST_CASE( "protocol" ) {
  run<bson::Serialize, std::string>();
}