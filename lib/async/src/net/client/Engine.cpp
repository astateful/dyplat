#include "astateful/async/net/client/Engine.hpp"
#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/client/Stream.hpp"

#include "net/Socket.hpp"

namespace astateful {
namespace async {
namespace net {
namespace client {
namespace {
  std::unique_ptr<Socket> generate_remote_socket( HANDLE cp,
                                                  const std::wstring& key,
                                                  int family ) {
    // Technically every address is valid and can only be invalidated when
    // trying to connect. However, we do not want to allow connections to
    // happen on default or weird interfaces so we check the address.
    auto address( generate_address( key ) );
    if ( !address ) return {};

    ADDRINFOW hint;
    memset( &hint, 0, sizeof( hint ) );
    hint.ai_family = family;
    hint.ai_socktype = SOCK_STREAM;

    // Populate the address info structure with information about the
    // address. This method can return multiple response interfaces and
    // we should try to listen on them all.
    PADDRINFOW list;
    if (!address->info( hint, list )) return {};

    for ( auto ai = list; ai != nullptr; ai = ai->ai_next ) {
      auto remote = std::make_unique<Socket>( ai );

      if ( WSAConnect( *remote,
                       ai->ai_addr,
                       static_cast<int>( ai->ai_addrlen ),
                       NULL,
                       NULL,
                       NULL,
                       NULL ) == SOCKET_ERROR ) {
        FreeAddrInfoW( list );
      };

      // Create a completion port using the remote as the handle and key.
      remote->createIOCP( cp );

      FreeAddrInfoW( list );

      return remote;
    }

    FreeAddrInfoW( list );

    return {};
  }
}

  Engine::Engine( const pipe::client::Engine& pipe,
                  const connection_t& connection ) :
    net::Engine( pipe ),
    m_connection( connection ),
    m_family( AF_INET) {}

  bool Engine::runIPv4() {
    WSADATA data;
    WORD version = MAKEWORD( 2, 2 );

    if ( WSAStartup( version, &data ) != 0 ) return false;

    return start();
  }

  bool Engine::Send( const std::wstring& key,
                     const Request& request,
                     Response* response ) const {
    auto remote_socket = generate_remote_socket( m_cp, key, m_family );
    if ( !remote_socket ) return false;

    // Unfortunately, at this point we have to pass on ownership of the
    // socket to the Windows completion port model, and trust that the
    // model, if done correctly, will release the socket.
    auto raw_socket = remote_socket.release();

    // The Windows completion port model also assumes ownership over the
    // raw connection that is created here as well.
    auto connection = m_connection.at( key )( response, request );
    if ( !connection->transfer( *raw_socket, m_pipe ) ) {
      delete raw_socket;
      delete connection;

      return false;
    }

    return true;
  }
}
}
}
}
