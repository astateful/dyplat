#include "astateful/async/net/server/Connection.hpp"

namespace astateful {
namespace async {
namespace net {
namespace server {
  Connection::Connection( const Address& local, const Address& remote ) :
    m_local( local ),
    m_remote( remote ),
    net::Connection() {
    m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
    m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
  }
}
}
}
}
