#include "Response/Get.hpp"

namespace astateful {
namespace cache {
  ResponseGet::ResponseGet( const std::string& key, uint64_t& cas ) :
    Response( state_e::VALUE ), m_key( key ), m_cas( cas ) {}

  bool ResponseGet::Accept( const char * data, int length ) {
    for ( int i = 0; i < length; i++ ) {
      switch ( data[i] ) {
      case '\r':
        switch ( m_state ) {
        case state_e::BYTES:
          m_num_bytes = std::stoi( m_buffer );
          m_buffer.clear();
          m_state = state_e::BYTES_STORED;
          break;
        case state_e::CAS:
          m_cas = std::stol( m_buffer );
          m_buffer.clear();
          m_state = state_e::BYTES_STORED;
          break;
        case state_e::DATA:
          if ( m_buffer.size() == m_num_bytes ) {
            m_state = state_e::END;

            return true;
          } else {
            m_buffer += data[i];
          }
          break;
        }
        break;
      case '\n':
        switch ( m_state ) {
        case state_e::BYTES_STORED:
          m_state = state_e::DATA;
          break;
        case state_e::DATA:
          m_buffer += data[i];
          break;
        }
        break;
      case ' ':
        switch ( m_state ) {
        case state_e::VALUE:
          if ( m_buffer == "VALUE" ) {
            m_buffer.clear();
            m_state = state_e::KEY;
          }
          break;
        case state_e::BYTES:
          m_num_bytes = std::stoi( m_buffer );
          m_buffer.clear();
          m_state = state_e::CAS;
          break;
        case state_e::KEY:
          if ( m_buffer != m_key ) return false;

          m_buffer.clear();
          m_state = state_e::FLAG;
          break;
        case state_e::FLAG:
          m_flags = std::stol( m_buffer );
          m_buffer.clear();
          m_state = state_e::BYTES;
          break;
        case state_e::DATA:
          m_buffer += data[i];
          break;
        }
        break;
      default:
        m_buffer += data[i];
        break;
      }
    }

    return false;
  }

  std::string ResponseGet::result() {
    while ( !m_done ) {}

    return m_buffer;
  }
}
}
