#include "astateful/async/Engine.hpp"

#include "Ws2tcpip.h"

namespace astateful {
namespace async {
  Engine::Engine() :
    m_consume(),
    m_cp( CreateIoCompletionPort(
                    INVALID_HANDLE_VALUE,
                    nullptr,
                    0,
                    0 ) ) {}

  Engine::~Engine() {
    for ( auto& thread : m_consume ) {
      // Wakeup all IOCP based threads and allow them to exit. This should
      // normally never fail so long as the CP is still valid.
      PostQueuedCompletionStatus( m_cp, 0, 0, 0 );
    }

    for ( auto& thread : m_consume ) {
      // Calling PostQueuedCompletionStatus could have already resulted
      // in some of the threads being terminated, in which case they cannot
      // be joined. Make sure that the thread is joinable before waiting on it.
      if ( thread.joinable() ) thread.join();
    }

    CloseHandle( m_cp );
  }
}
}