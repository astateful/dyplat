#define _WINSOCKAPI_
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "autogen/config.h"
#include "client/Context.hpp"
#include "client/Handle.hpp"
#include "astateful/ssl/server/Context.hpp"
#include "astateful/ssl/server/Handle.hpp"
#include "astateful/ssl/Init.hpp"

using namespace astateful;

TEST_CASE( "ssl" ) {
  const std::wstring pkey( install_prefix() + L"/ssl/ec/prime256v1/key.pem" );
  const std::wstring cert( install_prefix() + L"/ssl/ec/prime256v1/cert.pem" );

  ssl::Init ssl_init;
  ssl::server::Context ssl_server_context( cert, pkey, "http/1.1" );
  ssl::server::Handle ssl_server_handle( ssl_server_context, 512 );
  ssl::client::Context ssl_client_context( "http/1.1" );
  ssl::client::Handle ssl_client_handle( ssl_client_context, 512 );

  std::vector<uint8_t> pending;

  // Initiate the handshake process...
  ssl_client_handle.handshake();

  // Understand that in a perfect world the exact amount of pending
  // data could always be retrieved, however when dealing with buffers
  // of fixed sizes, the pending data must be flushed in chunks.
  while ( ssl_client_handle.flush( pending ) ) {
    ssl_server_handle.update( pending );
  }

  while ( ssl_server_handle.flush( pending ) ) {
    ssl_client_handle.update( pending );
  }

  while ( ssl_client_handle.flush( pending ) ) {
    ssl_server_handle.update( pending );
  }

  while ( ssl_server_handle.flush( pending ) ) {
    ssl_client_handle.update( pending );
  }

  auto x = ssl_server_handle.state();

  REQUIRE( strcmp( ssl_server_handle.state(), "SSLOK " ) == 0 );

  // Now either the server or client can send data. The client
  // will now send data to the server.
  auto res = ssl_client_handle.write( "Hello world!" );
  while ( ssl_client_handle.flush( pending ) ) {
    ssl_server_handle.update( pending );
  }

  std::string output( 4096, '0' );
  int read = ssl_server_handle.read( output );
  if ( read > 0 ) {
    output.resize( read );

    REQUIRE( output == "Hello world!" );
  }
}
