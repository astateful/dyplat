#include "pipe/Connection.hpp"

#include <cassert>

namespace astateful {
namespace async {
namespace pipe {
  Connection::Connection( flag_e flag ) :
    m_flag( flag ),
    async::Connection() {}

  Connection::Connection( flag_e flag, const std::vector<uint8_t>& buffer ) :
    async::Connection( buffer ),
    m_flag( flag ) {}

  bool Connection::transfer( HANDLE handle ) {
    if ( m_flag == flag_e::READ ) {
      return read( handle );
    } else if ( m_flag == flag_e::WRITE ) {
      return write( handle );
    } else if ( m_flag == flag_e::DONE ) {
      assert( CloseHandle( handle ) );

      return false;
    }

    return false;
  }

  int Connection::write( HANDLE handle ) {
    ZeroMemory( &overlapped, sizeof( WSAOVERLAPPED ) );

    if ( WriteFile( handle,
      &m_buffer[0],
      m_buffer.size(),
      nullptr,
      &( overlapped ) ) == TRUE ) return true;

    auto last_error = GetLastError();
    switch ( last_error ) {
      case ERROR_IO_PENDING:
        return true;
      break;
      case ERROR_INVALID_HANDLE:
        return false;
      break;
      default:
        assert( false );
      break;
    }

    return false;
  }

  int Connection::read( HANDLE handle ) {
    ZeroMemory( &overlapped, sizeof( WSAOVERLAPPED ) );

    if ( ReadFile( handle,
      &m_buffer[0],
      m_buffer.size(),
      nullptr,
      &( overlapped ) ) == TRUE ) return true;

    auto last_error = GetLastError();
    switch ( last_error ) {
      case ERROR_IO_PENDING:
        return true;
      break;
      case ERROR_INVALID_HANDLE:
        return false;
      break;
      // This is triggered when a fake event is sent when shutting down
      // server; we are waiting for a process that will never take place
      // so return false.
      case ERROR_PIPE_LISTENING:
        return false;
      break;
      default:
        assert( false );
      break;
    }

    return false;
  }
}
}
}
