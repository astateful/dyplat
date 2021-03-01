#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/token/h1/Context.hpp"
#include "astateful/token/h1/Machine.hpp"
#include "astateful/token/graph/Context.hpp"
#include "astateful/token/graph/Machine.hpp"
#include "astateful/token/address/Context.hpp"
#include "astateful/token/address/Machine.hpp"

using namespace astateful;
using namespace astateful::token;

using query_t = std::vector<std::pair<std::string, std::string>>;
using header_t = std::vector<std::pair<std::string, std::string>>;

void h1_query( bool expected, const std::string& input, const query_t check = {} ) {
  const h1::Machine machine;
  h1::Context context( machine.initialState() );
  bool success = true;

  for ( const auto& c : input ) {
    if ( !machine( context, context.state, c ) ) {
      success = false;
      break;
    }
  }

  REQUIRE( success == expected );
  if ( success == true ) {
    REQUIRE( context.state == machine.successState() );
    REQUIRE( check == context.query );
  }
}

void h1_header( bool expected, const std::string& input, const header_t check = {} ) {
  const h1::Machine machine;
  h1::Context context( machine.initialState() );
  bool success = true;

  for ( const auto& c : input ) {
    if ( !machine( context, context.state, c ) ) {
      success = false;
      break;
    }
  }

  REQUIRE( success == expected );
  if ( success == true ) {
    REQUIRE( context.state == machine.successState() );
    REQUIRE( check == context.header );
  }
}

void h1_method( bool expected, const std::string& input, const std::string check = {} ) {
  const h1::Machine machine;
  h1::Context context( machine.initialState() );
  bool success = true;

  for ( const auto& c : input ) {
    if ( !machine( context, context.state, c ) ) {
      success = false;
      break;
    }
  }

  REQUIRE( success == expected );
  if ( success == true ) {
    REQUIRE( context.state == machine.successState() );
    REQUIRE( check == context.method );
  }
}

void h1_body( bool expected, const std::string& input, const std::string check = {} ) {
  const h1::Machine machine;
  h1::Context context( machine.initialState() );
  bool success = true;

  for ( const auto& c : input ) {
    if ( !machine( context, context.state, c ) ) {
      success = false;
      break;
    }
  }

  REQUIRE( success == expected );
  if ( success == true ) {
    if ( context.body != "" )
      REQUIRE( context.state == machine.successState() );

    REQUIRE( check == context.body );
  }
}

TEST_CASE( "h1" ) {
  SECTION( "method" ) {
    h1_method( false, "SOMETHING uri HTTP/1.1\r\n\r\n" );
    h1_method( true, "GET uri HTTP/1.1\r\n\r\n", "GET" );
    h1_method( true, "POST uri HTTP/1.1\r\n\r\n", "POST" );
    h1_method( true, "PUT uri HTTP/1.1\r\n\r\n", "PUT" );
    h1_method( true, "DELETE uri HTTP/1.1\r\n\r\n", "DELETE" );
  }

  SECTION( "query" ) {
    h1_query( false, "POST asd? HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?a HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?& HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?=& HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?=&= HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?=&& HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?== HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?==& HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?=a=b& HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?a&b=c HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?b=&c HTTP/1.1\r\n\r\n" );
    h1_query( false, "POST asd?b&=d&c HTTP/1.1\r\n\r\n" );
    h1_query( true, "POST asd?a= HTTP/1.1\r\n\r\n", { { "a", "" } } );
    h1_query( true, "POST asd?= HTTP/1.1\r\n\r\n", {} );
    h1_query( true, "POST asd?=b HTTP/1.1\r\n\r\n", { { "", "b" } } );
    h1_query( true, "POST asd?b=c HTTP/1.1\r\n\r\n", { { "b", "c" } } );
    h1_query( true, "POST asd?a=b&c=d HTTP/1.1\r\n\r\n", { { "a", "b" }, { "c", "d" } } );
    h1_query( true, "POST asd?%26=%26&%2F=%5D HTTP/1.1\r\n\r\n", { { "&", "&" }, { "/", "]" } } );
    h1_query( true, "POST asd?%26= HTTP/1.1\r\n\r\n", { { "&", "" } } );
  }

  SECTION( "headers" ) {
    h1_header( false, "POST asd?a=b HTTP/1.1\r\n:\r\n\r\n" );
    h1_header( false, "POST asd?a=b HTTP/1.1\r\nAccept all\r\n\r\n" );
    h1_header( false, "POST asd?a=b HTTP/1.1\r\nAccept all:\r\n\r\n" );
    h1_header( true, "POST asd?a=b HTTP/1.1\r\nAccept: all\r\nAccept-Charset: UTF-8\r\n\r\n",
                { { "Accept", "all" }, { "Accept-Charset", "UTF-8" } } );
  }

  SECTION( "body" ) {
    h1_body( false, "POST asd?a=b HTTP/1.1\r\n\r\ntest", "test" );
    h1_body( true, "POST asd?a=b HTTP/1.1\r\nContent-Length: 4\r\n\r\n" );
    h1_body( true, "POST asd?a=b HTTP/1.1\r\nContent-Length: 4\r\n\r\ntest", "test" );
  }
}

void graph_test( bool expected, const std::string& input, const std::string document = "",
                 const std::string connection = "" ) {
  const graph::Machine machine;
  graph::Context context( machine.initialState() );
  bool success = true;

  for ( const auto& c : input ) {
    if ( !machine( context, context.state, c ) ) {
      success = false;
      break;
    }
  }

  REQUIRE( success == expected );
  if ( success == true ) {
    REQUIRE( context.state == machine.successState() );
    REQUIRE( context.document == document );
    REQUIRE( context.connection == connection );
  }
}

TEST_CASE( "graph") {
  graph_test( true, "/abcdefghijklmnop12345678/posts", "abcdefghijklmnop12345678", "posts" );
  graph_test( true, "/abcdefghijklmnop12345678", "abcdefghijklmnop12345678" );
}

void address_test( bool expected, const std::wstring& input, const std::wstring hostname = L"",
                   const std::wstring port = L"" ) {
  const address::Machine machine;
  address::Context context( machine.initialState() );
  bool success = true;

  for ( const auto& c : input ) {
    if ( !machine( context, context.state, c ) ) {
      success = false;
      break;
    }
  }

  REQUIRE( success == expected );
  if ( success == true ) {
    REQUIRE( context.state == machine.successState() );
    REQUIRE( context.hostname == hostname );
    REQUIRE( context.port == port );
  }
}

TEST_CASE( "address") {
  address_test( true, L"localhost:12345", L"localhost", L"12345" );
  address_test( true, L"l:1", L"l", L"1" );
  address_test( false, L":12345" );
}