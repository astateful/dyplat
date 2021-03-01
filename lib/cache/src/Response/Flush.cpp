#include "Response/Flush.hpp"

namespace astateful {
namespace cache {
  ResponseFlush::ResponseFlush() : Response( state_e::BODY ) {}

  bool ResponseFlush::result() {
    while ( !m_done ) {}

    if ( m_buffer == "OK" ) return true;

    return false;
  }
}
}
