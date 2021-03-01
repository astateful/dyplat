#include "astateful/async/net/client/Request.hpp"

namespace astateful {
namespace async {
namespace net {
namespace client {
  Request::Request( const std::vector<uint8_t>& buffer ) :
    m_buffer( buffer ) {}

  const std::vector<uint8_t>& Request::buffer() const {
    return m_buffer;
  }
}
}
}
}