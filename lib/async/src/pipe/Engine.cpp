#include "astateful/async/pipe/Engine.hpp"

#include "pipe/Connection.hpp"

#include <ws2tcpip.h>
#include <mstcpip.h>

namespace astateful {
namespace async {
namespace pipe {
  void Engine::consume( HANDLE cp ) {
    DWORD transferred;
    HANDLE * handle = nullptr;
    OVERLAPPED * overlapped = nullptr;
    Connection * connection = nullptr;

    while ( true ) {
      if ( GetQueuedCompletionStatus( cp,
                                      &transferred,
                                      reinterpret_cast<PULONG_PTR>( &handle ),
                                      &overlapped,
                                      INFINITE ) == 0 ) return;

      if ( overlapped == nullptr && transferred == 0 && handle == nullptr )
        return;

      connection = CONTAINING_RECORD( overlapped, Connection, overlapped );

      if ( transferred == 0 ) {
        //delete connection;

        continue;
      }

      if ( !connection->update( transferred ) ) {
        int halt = 0;
      }

      if ( !connection->transfer( handle ) ) {
        delete connection;
      }
    }
  }

  bool Engine::start() {
    for ( size_t i = 0; i < 2; ++i ) {
      m_consume.emplace_back( consume, m_cp );
    }

    return true;
  }
}
}
}
