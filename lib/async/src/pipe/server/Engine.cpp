#include "astateful/async/pipe/server/Engine.hpp"

#include "pipe/server/Connection.hpp"

#include <ws2tcpip.h>
#include <mstcpip.h>
#include <WinBase.h>

#include <thread>
#include <vector>
#include <iostream>
#include <cassert>

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

namespace astateful {
namespace async {
namespace pipe {
namespace server {
namespace {
  HANDLE create_instance(const std::string& name ) {
      auto handle = CreateNamedPipe( name.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFSIZE*sizeof( TCHAR ),
        BUFSIZE*sizeof( TCHAR ),
        PIPE_TIMEOUT,
        NULL );

      return handle;
  }

  bool connect_instance( HANDLE handle, LPOVERLAPPED ol ) {
    bool pending_io = false;

    // An overlapped call to ConnectNamedPipe should always return false
    // so if it returns true we should return false...make sense?
    if ( ConnectNamedPipe( handle, ol ) ) return false;

    auto last_error = GetLastError();
    switch ( last_error ) {
      case ERROR_IO_PENDING:
        pending_io = true;
        return true;
      break;
      case ERROR_PIPE_CONNECTED:
        SetEvent( ol->hEvent );
        return true;
      break;
      default:
        assert( false );
      break;
    }

    return pending_io;
  }

  void produce( HANDLE cp, const callback_t& m_callback, LPOVERLAPPED ol_connect ) {
    for ( const auto& callback : m_callback ) {
      std::string absolute_pipe = "\\\\.\\pipe\\" + callback.first;

      // Create an event object just for the connect operation.
      auto connect_ev = CreateEvent( NULL, TRUE, TRUE, NULL );
      if ( connect_ev == NULL ) return;
 
      // Declare an overlapped handler for the connection. We set the
      // lower bit in order to prevent the event from awakening a pending
      // call with GetQueuedCompletionStatus.
      ol_connect->hEvent = ((HANDLE)((DWORD)connect_ev|0x1));

      // Create the named pipe handle instance.
      auto handle = create_instance( absolute_pipe.c_str() );
      if ( handle == INVALID_HANDLE_VALUE ) return;

      // Perform the actual connection.
      bool pending_io = connect_instance( handle, ol_connect );

      // Associate the newly created named pipe handle to the original
      // completion port. This IOCP will handle all overlapped requests
      // except for the connection request.
      CreateIoCompletionPort( reinterpret_cast<HANDLE>( handle ),
                              cp,
                              reinterpret_cast<DWORD>( handle ),
                              0 );

      DWORD wait, transferred;

      while ( true ) { 
        // Wait for the event to be in the signaled state.
        wait = WaitForSingleObjectEx( connect_ev, INFINITE, TRUE );

        switch ( wait ) {
          case 0: {
            if ( pending_io ) {
              // Wait for the result of the overlapped connect operation. This
              // is a blocking call, which is why we run this in a separate
              // thread in the first place.
              auto success = GetOverlappedResult( handle,
               ol_connect,
               &transferred,
               TRUE );

              if ( !success ) {
                auto last_error = GetLastError();
                return;
              }
            }

            // Fire away a connection to handle the current handle instance.
            auto connection = new Connection( callback.second );
            if ( !connection->transfer( handle ) ) {
              delete connection;
              return;
            }

            // Create the next named pipe instance.
            handle = create_instance( absolute_pipe.c_str() );
            if ( handle == INVALID_HANDLE_VALUE ) return;

            // Perform the actual connection.
            pending_io = connect_instance( handle, ol_connect );

            // Associate the newly created named pipe handle to the original
            // completion port. This IOCP will handle all overlapped requests
            // except for the connection request.
            CreateIoCompletionPort( reinterpret_cast<HANDLE>( handle ),
                                    cp,
                                    reinterpret_cast<DWORD>( handle ),
                                    0 );
          }
          break;
          case WAIT_IO_COMPLETION:
          break;
          default: {
            printf("WaitForSingleObjectEx (%d)\n", GetLastError()); 
            return;
          }
        }
      }
    }
  }
}

  Engine::Engine( const callback_t& callback ) :
    pipe::Engine(),
    m_handle(),
    m_callback( callback ),
    m_produce() {}

  bool Engine::start() {
    if ( !pipe::Engine::start() ) return false;

    m_produce.emplace_back( produce, m_cp, m_callback, &m_overlapped );

    return true;
  }

  void Engine::kill() {
    // Send a fake event to the overlapped structure to place it in a
    // signaled state, since the producer is sleeping right now.
    SetEvent( m_overlapped.hEvent );
  }

  Engine::~Engine() {
    for ( auto& thread : m_produce ) {
      if ( thread.joinable() ) thread.join();
    }
  }
}
}
}
}