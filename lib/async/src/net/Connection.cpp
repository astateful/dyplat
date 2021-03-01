#include "astateful/async/net/Connection.hpp"

namespace astateful {
namespace async {
namespace net {
  Connection::Connection() :
    m_flag( flag_e::RECV ),
    async::Connection() {}

  Connection::Connection( const std::vector<uint8_t>& buffer ) :
    m_flag( flag_e::SEND ),
    async::Connection( buffer ) {
    m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
    m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
  }

  bool Connection::update( DWORD transferred,
                           const async::pipe::client::Engine& pipe ) {

    switch ( m_flag ) {
      case flag_e::RECV: return recvEvent( transferred, pipe ); break;
      case flag_e::SEND: return sendEvent( transferred, pipe ); break;
    }

    return false;
  }

  bool Connection::transfer( SOCKET socket,
                             const pipe::client::Engine& pipe ) {
    ZeroMemory( &overlapped, sizeof( WSAOVERLAPPED ) );

    // This branch will create a new socket operation depending on the new
    // value of the flag.
    if ( m_flag == flag_e::SEND ) {
      if ( !send( socket ) )
        {
          int x = WSAGetLastError();
          return false;
        }
    } else if ( m_flag == flag_e::RECV ) {
      if ( !recv( socket ) ) {
          int x = WSAGetLastError();
        return false;
        }
    }

    return true;
  }

  bool Connection::recv( SOCKET socket )
  {
    DWORD flag = 0;

    if ( WSARecv( socket,
                  &m_wsa_buf,
                  1,
                  nullptr,
                  &flag,
                  &overlapped,
                  nullptr
                ) == SOCKET_ERROR ) {
      if ( WSAGetLastError() != ERROR_IO_PENDING ) return false;
    }

    return true;
  }

  bool Connection::send( SOCKET socket )
  {
    DWORD flag = 0;

    if ( WSASend( socket,
                  &m_wsa_buf,
                  1,
                  nullptr,
                  flag,
                  &overlapped,
                  nullptr
                ) == SOCKET_ERROR ) {
      if ( WSAGetLastError() != ERROR_IO_PENDING ) return false;
    }

    return true;
  }
}
}
}
