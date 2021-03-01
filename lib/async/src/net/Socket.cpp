#include "net/Socket.hpp"

#include <cassert>

namespace astateful {
namespace async {
namespace net {
  Socket::Socket( SOCKET&& socket, const SOCKADDR_STORAGE& address ) :
    m_socket( std::move( socket ) ),
    m_address( address ) {}

  Socket::Socket( const ADDRINFOW * ai ) :
    m_socket( WSASocketW( ai->ai_family,
                          ai->ai_socktype,
                          ai->ai_protocol,
                          nullptr,
                          0,
                          WSA_FLAG_OVERLAPPED ) ),
    m_address( ai ) {
    assert( m_socket != INVALID_SOCKET );
  }

  Socket::Socket( Socket&& rhs ) :
    m_socket( std::move (rhs.m_socket )),
    m_address( std::move( rhs.m_address ) ) {}

  void Socket::createIOCP( HANDLE iocp ) {
    CreateIoCompletionPort(
      reinterpret_cast<HANDLE>( m_socket ),
      iocp,
      reinterpret_cast<ULONG_PTR>( this ),
      0 );
  }

  Socket::operator const SOCKET&() const {
    return m_socket;
  }

  Socket::operator const Address&() const {
    return m_address;
  }

  Socket::~Socket() {
    if ( m_socket == INVALID_SOCKET ) return;

    shutdown( m_socket, SD_BOTH );

    closesocket( m_socket );
  }
}
}
}
