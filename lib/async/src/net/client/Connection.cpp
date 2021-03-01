#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/net/client/Response.hpp"
#include "astateful/async/net/client/Request.hpp"

#include <iostream>
#include <string>

namespace astateful {
namespace async {
namespace net {
namespace client {
  Connection::Connection( Response * response, const Request& request ) :
    m_response( response ),
    net::Connection( request.buffer() ) {}

  bool Connection::recvEvent( DWORD transferred,
                              const pipe::client::Engine& pipe ) {
    if ( m_response->Accept( m_wsa_buf.buf, transferred ) ) {
      // Since this connection will not be used again, send an empty send
      // request so that the class can be destroyed.
      m_flag = flag_e::SEND;
      m_buffer.clear();
      m_wsa_buf.len = 0;
      m_wsa_buf.buf = nullptr;

      m_response->setDone( true ); // tell anyone sleeping to wake up...
    }

    return true;
  }

  bool Connection::sendEvent( DWORD transferred,
                              const pipe::client::Engine& pipe ) {
    // The length is incremented on every send by the amount that was
    // transferred. So the original length should always be the size of
    // the entire response for the increment to work.
    if ( transferred - m_wsa_buf.len == 0 ) {
      // After everything has been sent, we assume we will receieve new
      // data, unless we do not have something to handle a response. In
      // that case, do a fake send so this class can be destroyed.
      if ( m_response == nullptr ) {
        m_flag = flag_e::SEND;
        m_buffer.clear();
        m_wsa_buf.len = 0;
        m_wsa_buf.buf = nullptr;
      } else {
        m_flag = flag_e::RECV;
        m_buffer.clear();
        m_buffer.resize( maxBufferSize );
        m_wsa_buf.len = maxBufferSize;
        m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
      }
    } else {
      m_wsa_buf.buf += transferred;
      m_wsa_buf.len -= transferred;
    }

    return true;
  }
}
}
}
}
