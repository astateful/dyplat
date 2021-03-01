#include "Response/Incdec.hpp"

namespace astateful {
namespace cache {
  ResponseIncdec::ResponseIncdec() : Response( state_e::BODY ) {}

  std::string ResponseIncdec::result() {
    while ( !m_done ) {}

    if ( m_buffer == "NOT_FOUND" ) return "";

    return m_buffer;
  }
}
}
