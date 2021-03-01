#include "astateful/async/net/Engine.hpp"
#include "astateful/async/net/Connection.hpp"
#include "astateful/async/pipe/client/Stream.hpp"
#include "astateful/async/pipe/client/Engine.hpp"

#include "net/Socket.hpp"

#include <ws2tcpip.h>
#include <mstcpip.h>

namespace astateful {
namespace async {
namespace net {
  Engine::Engine( const pipe::client::Engine& pipe ) :
    m_pipe( pipe ),
    async::Engine() {}

  void Engine::consume( HANDLE cp, const pipe::client::Engine& pipe ) {
    DWORD transferred;
    Socket * socket = nullptr;
    OVERLAPPED * overlapped = nullptr;
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER finish;

    while ( true ) {
      if ( GetQueuedCompletionStatus( cp,
                                      &transferred,
                                      reinterpret_cast<PULONG_PTR>( &socket ),
                                      &overlapped,
                                      INFINITE ) == 0 ) return;

      auto log = pipe::client::Stream( pipe, "log" );

      // This is our special termination signal check. When this signal is
      // received, we stop the thread since there is no more work to process.
      if ( overlapped == nullptr && transferred == 0 && socket == nullptr )
        return;

      auto connection = CONTAINING_RECORD( overlapped, Connection, overlapped );

      // Here we perform some cleanup. Note that upon handle destruction the
      // socket will be freed.
      if ( transferred == 0 ) {
        log << "deleting connection" << Address( *socket );
        log << pipe::client::noack;

        delete socket;
        delete connection;

        continue;
      }

      QueryPerformanceFrequency( &frequency );
      QueryPerformanceCounter( &start );

      if ( !connection->update( transferred, pipe ) ) {
        log << "deleting connection" << Address( *socket );
        log << pipe::client::noack;

        delete socket;
        delete connection;

        continue;
      }

      QueryPerformanceCounter( &finish );

      double time = ( finish.QuadPart - start.QuadPart ) ;
      time /= static_cast<double>(frequency.QuadPart);

      log << "execution took " << time << pipe::client::noack;
      
      if ( !connection->transfer( *socket, pipe ) ) {
        log << "deleting connection" << Address( *socket );
        log << pipe::client::noack;

        delete socket;
        delete connection;
      }
    }
  }

  bool Engine::start() {
    SYSTEM_INFO info;
    GetSystemInfo( &info );

    auto num_consumer = info.dwNumberOfProcessors * 2;
    m_consume.reserve( num_consumer );
    for ( size_t i = 0; i < num_consumer; ++i )
      m_consume.emplace_back( consume, m_cp, std::ref( m_pipe ) );

    return true;
  }
}
}
}