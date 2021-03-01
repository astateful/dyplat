#define _WINSOCKAPI_
#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/async/pipe/client/Stream.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/server/Engine.hpp"

#include <mutex>

using namespace astateful;
using namespace astateful::async;

void exec_request( const pipe::client::Engine& pipe, int i ) {
  pipe::client::Stream stream( pipe, "test" );
  std::string test = "thread_data_" + std::to_string( i );

  REQUIRE( pipe.send( "test", test, true ) );
}

TEST_CASE( "pipe" ) {
  int num_callbacks_exec = 0;

  std::mutex mutex;
  std::unique_lock<std::mutex> lock( mutex, std::defer_lock );

  pipe::server::callback_t callback;
  callback["test"] = [&]( const std::vector<uint8_t>& buffer ) {
    lock.lock();
    ++num_callbacks_exec;

    /*std::string data( buffer.begin()+1, buffer.end() );

    std::cout << "server beg ------" << std::endl;
    std::cout << data << std::endl;
    if (buffer[0] == 1) {
      std::cout << "ack" << std::endl;
    } else {
      std::cout << "noack" << std::endl;
    }
    std::cout << "server end------" << std::endl << std::endl;*/

    lock.unlock();
  };

  pipe::server::Engine server( callback );
  pipe::client::Engine client;

  server.start();
  client.start();

  pipe::client::Stream stream( client, "test" );
  stream << "stream1" << "stream4" << pipe::client::ack;
  stream << "stream2" << "stream5" << pipe::client::noack;
  stream << "stream3" << "stream6" << pipe::client::ack;

  REQUIRE( client.send( "test", "data1", false ) );
  REQUIRE( client.send( "test", "data2", false ) );
  REQUIRE( client.send( "test", "data3", true ) );

  std::vector<std::thread> request( 20 );

  for ( size_t i = 0; i < request.size(); i++ ) {
    request[i] = std::thread( exec_request, std::ref( client ), i );
  }

  for ( auto& thread : request ) thread.join();

  // Block here since the main thread will reach here before any
  // of the other request threads that were spawned.
  while ( num_callbacks_exec != 26 ) {}

  REQUIRE( num_callbacks_exec == 26 );

  server.kill();
}