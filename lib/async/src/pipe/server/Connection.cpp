#include "pipe/server/Connection.hpp"

namespace astateful {
namespace async {
namespace pipe {
namespace server {
  Connection::Connection( std::function<void( const std::vector<uint8_t>& )> generator ) :
    async::pipe::Connection( flag_e::READ ),
    m_generator( generator ) {
  }

  bool Connection::update( DWORD transferred ) {
    switch ( m_flag ) {
      case flag_e::READ: {
        m_generator( { m_buffer.begin(), m_buffer.begin() + transferred } );

        if ( m_buffer[0] == 0 ) {
          m_flag = flag_e::DONE;

          m_buffer.clear();
          m_buffer.resize( maxBufferSize );
          //DisconnectNamedPipe( handle );
        } else { 
          m_flag = flag_e::WRITE;

          m_buffer.clear();
          m_buffer.push_back( 1 );
        }
      }
      break;
      case flag_e::WRITE: {
        if ( m_buffer.size() == transferred ) {
          m_flag = flag_e::DONE;

          m_buffer.clear();
          m_buffer.resize( maxBufferSize );
          //DisconnectNamedPipe( handle );
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
