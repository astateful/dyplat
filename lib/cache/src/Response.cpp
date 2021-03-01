#include "Response.hpp"
#include "astateful/cache/Context.hpp"

#include <string>

namespace astateful {
namespace cache {
  Response::Response( state_e state ) : m_state( state ) {}

  bool Response::Accept( const char * data, int length ) {
    for ( int i = 0; i < length; i++ ) {
      switch ( m_state ) {
        case state_e::BODY:
          if ( data[i] == '\r') {
            m_state = state_e::END;
          } else {
            m_buffer += data[i];
          }
        break;
        case state_e::END:
          if ( data[i] == '\n' ) return true;
        break;
      }
    }

    return false;
  }
}
}
