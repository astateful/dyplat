#include "astateful/async/pipe/client/Engine.hpp"

#include "pipe/client/Connection.hpp"

#include <cassert>

#include "Ws2tcpip.h"

namespace astateful {
namespace async {
namespace pipe {
namespace client {
  bool Engine::send( const std::string& pipe,
                     const std::string& buffer,
                     bool ack ) const {
    std::string absolute_pipe = "\\\\.\\pipe\\" + pipe;
    HANDLE handle;
    DWORD last_error;

    // Need to wait to open a named pipe in the case that the server
    // has no yet created a pipe but a request is made.
    while ( true ) {
      handle = CreateFile( absolute_pipe.c_str(),
                           GENERIC_WRITE | GENERIC_READ,
                           0,
                           NULL,
                           OPEN_EXISTING,
                           FILE_FLAG_OVERLAPPED,
                           NULL );

      if ( handle != INVALID_HANDLE_VALUE ) break;

      last_error = GetLastError();
      switch ( last_error ) {
        // Wait until the pipe becomes free.
        case ERROR_PIPE_BUSY:
        break;
        // The pipe could basically not exist.
        case ERROR_FILE_NOT_FOUND:
        break;
        default:
        assert( false );
        break;
      }
    }

    CreateIoCompletionPort( reinterpret_cast<HANDLE>( handle ),
                            m_cp,
                            reinterpret_cast<ULONG_PTR>( handle ),
                            0 );

    // Data is read from the pipe as a stream of bytes. This mode is
    // the default if no read-mode flag is specified.
    DWORD mode = PIPE_READMODE_MESSAGE & PIPE_WAIT;
    if ( !SetNamedPipeHandleState( handle, &mode, nullptr, nullptr ) )
      return false;

    std::vector<uint8_t> buffer_copy;
    buffer_copy.push_back( ack ? '\x1' : '\x0' );
    buffer_copy.insert(buffer_copy.end(), buffer.begin(), buffer.end() );

    auto connection = new Connection( buffer_copy, ack );
    if ( !connection->transfer( handle ) ) {
      delete connection;
      return false;
    }

    return true;
  }
}
}
}
}
