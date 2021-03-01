#include "Response/Store.hpp"

#include "astateful/cache/Context.hpp"

namespace astateful {
namespace cache {
  ResponseStore::ResponseStore() : Response( state_e::BODY ) {}

  result_e ResponseStore::result() {
    while ( !m_done ) {}

    if ( m_buffer == "STORED" ) return result_e::OK;
    if ( m_buffer == "EXISTS" ) return result_e::NOT_STORED;
    if ( m_buffer == "NOT_STORED" ) return result_e::NOT_STORED;
    if ( m_buffer == "ERROR" ) return result_e::NOT_STORED;

    return result_e::UNKNOWN;
  }
}
}
