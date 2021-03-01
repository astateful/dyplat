#include "astateful/async/net/server/Engine.hpp"
#include "astateful/async/net/server/Connection.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/client/Stream.hpp"

#include "net/Socket.hpp"

#include <iostream>
#include <string>

#include <ws2tcpip.h>
#include <mstcpip.h>

namespace astateful {
namespace async {
namespace net {
namespace server {
namespace {
  std::unique_ptr<Socket> generate_remote_socket( Socket& local ) {
    SOCKADDR_STORAGE address;
    int size = sizeof( address );

    auto remote = WSAAccept( local,
                             reinterpret_cast<SOCKADDR *>( &address ),
                             &size,
                             nullptr,
                             0 );

    if ( remote == SOCKET_ERROR ) return nullptr;

    return std::make_unique<Socket>( std::move( remote ), address );
  }

  void produce( HANDLE cp,
                Socket& local,
                const pipe::client::Engine& pipe,
                const bool& alive,
                const generator_t& generator ) {
    while ( alive ) {
      // Bear in mind that this method will block on this thread until a new
      // socket is accepted, so if we would already create the fs context at
      // this point, we could never open the file to see the contents because
      // the handle would never be released.
      auto remote_socket = generate_remote_socket( local );
      if ( !remote_socket ) return;

      // Do some logging...
      auto log = pipe::client::Stream( pipe, "log" );
      log << "local: " << Address( local ) << pipe::client::noack;
      log << "remote: " << Address( *remote_socket ) << pipe::client::noack;

      // Create a completion port using the remote as the handle and key.
      remote_socket->createIOCP( cp );

      // The Windows completion port model also assumes ownership over the
      // raw connection that is created here as well.
      auto connection = generator( local, *remote_socket );

      // Unfortunately, at this point we have to pass on ownership of the
      // socket to the Windows completion port model, and trust that the
      // model, if done correctly, will release the socket.
      auto raw_socket = remote_socket.release();

      // Initiate a completion packet by transferring on the socket.
      if ( !connection->transfer( *raw_socket, pipe ) ) {
        log << "connection transfer failed for " << Address( *raw_socket );
        log << pipe::client::noack;

        delete connection;
        delete raw_socket;

        return;
      }
    }
  }
}

  Engine::Engine( const pipe::client::Engine& pipe,
                  const connection_t& connection ) :
    net::Engine( pipe ),
    m_connection( connection ),
    m_alive( true ),
    m_produce() {}

  bool Engine::runIPv4() {
    return run( AF_INET );
  }

  bool Engine::runIPv6() {
    return run( AF_INET6 );
  }

  bool Engine::run( int family ) {
    WSADATA data;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &data ) != 0 ) return false;

    // Used for interacting with Windows setters.
    int on = 1;
    int off = 0;

    for ( const auto& connection : m_connection ) {
      // Technically every address is valid and can only be invalidated when
      // trying to connect. However, we do not want to allow connections to
      // happen on default or weird interfaces so we check the address.
      auto address( generate_address( connection.first ) );
      if ( !address ) return false;

      ADDRINFOW hint;
      memset( &hint, 0, sizeof( hint ) );
      hint.ai_family = family;
      hint.ai_socktype = SOCK_STREAM;
      hint.ai_flags = AI_PASSIVE;

      PADDRINFOW list;
      if ( !address->info( hint, list ) ) return false;

      for ( auto ai = list; ai != nullptr; ai = ai->ai_next ) {
        auto socket = std::make_unique<Socket>( ai );

        if ( setsockopt( *socket,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         reinterpret_cast<const char *>( &on ),
                         sizeof( on ) ) == SOCKET_ERROR ) return false;

        if ( setsockopt( *socket,
                         SOL_SOCKET,
                         SO_KEEPALIVE,
                         reinterpret_cast<const char *>( &on ),
                         sizeof( on ) ) == SOCKET_ERROR ) return false;

        if ( ai->ai_family == AF_INET6 ) {
          // Make sure that in case of IPV6 that we only deal with IPV6 addresses.
          if ( setsockopt( *socket,
                           IPPROTO_IPV6,
                           IPV6_V6ONLY,
                           reinterpret_cast<const char *>( &off ),
                           sizeof( off ) ) == SOCKET_ERROR ) return false;

          // Make sure that IPv6 addresses have a valid scope id.
          if ( IN6_IS_ADDR_LINKLOCAL( ( IN6_ADDR * )INETADDR_ADDRESS( ai->ai_addr ) ) &&
                ( ( ( SOCKADDR_IN6 * )( ai->ai_addr ) )->sin6_scope_id == 0 )
              ) return false;
        }

        if ( bind( *socket,
                   ai->ai_addr,
                   static_cast<int>( ai->ai_addrlen ) ) == SOCKET_ERROR ) return false;

        // Begin listening on the socket with a small backlog. We may need to
        // change this in the future.
        if ( listen( *socket, 5 ) == SOCKET_ERROR ) return false;

        // Store the listening socket so that threads in other scopes can
        // continually reference it to make new connections.
        m_socket.push_back( std::move( socket ) );

        // We also associate a thread to this socket which acts as a producer
        // by generating new connections for accepted sockets.
        m_produce.emplace_back( produce,
                                m_cp,
                                std::ref( *m_socket.back() ),
                                std::ref( m_pipe ),
                                std::ref( m_alive ),
                                connection.second );
      }

      FreeAddrInfoW( list );
    }

    return start();
  }

  void Engine::kill() {
    m_alive = false;
  }

  Engine::~Engine() {
    while (m_alive) {}

    for ( auto& thread : m_produce ) {
      if ( thread.joinable() ) thread.join();
    }

    m_socket.clear();

    WSACleanup();
  }
}
}
}
}
