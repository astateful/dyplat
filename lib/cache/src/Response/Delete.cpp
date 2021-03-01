#include "Response/Delete.hpp"

#include "astateful/cache/Context.hpp"

namespace astateful {
namespace cache {
  ResponseDelete::ResponseDelete() : Response( state_e::BODY ) {}

  result_e ResponseDelete::result() {
    while ( !m_done ) {}

    if ( m_buffer == "DELETED" ) return result_e::OK;
    if ( m_buffer == "NOT_FOUND" ) return result_e::NOT_FOUND;

    return result_e::UNKNOWN;
  }
}
}
