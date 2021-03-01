#include "pipe/client/Connection.hpp"

#include <iostream>

namespace astateful {
namespace async {
namespace pipe {
namespace client {
  Connection::Connection( const std::vector<uint8_t>& buffer, bool ack ) :
    async::pipe::Connection( flag_e::WRITE, buffer ),
    m_ack( ack ) {}

  bool Connection::update( DWORD transferred ) {
    switch ( m_flag ) {
      case flag_e::WRITE: {
        if ( transferred == m_buffer.size() ) {
          // If we are in ack mode, then we need to switch to read
          // mode since we expect a reply from the server.
          if ( m_ack ) {
            m_flag = flag_e::READ;

            m_buffer.clear();
            m_buffer.resize( maxBufferSize );
          } else {
            m_flag = flag_e::DONE;
          }
        }
      }
      break;
      case flag_e::READ: {
        if ( transferred == 1 ) {
          m_flag = flag_e::DONE;
        } else{
          int halt = 0;
        }
      }
      break;
    }

    return true;
  }
}
}
}
}
