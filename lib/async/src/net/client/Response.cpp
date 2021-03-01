#include "astateful/async/net/client/Response.hpp"

namespace astateful {
namespace async {
namespace net {
namespace client {
  void Response::setDone( bool done ) {
    m_done = done;
  }

  Response::operator bool() const {
    return m_done;
  }
}
}
}
}