#include "Response.hpp"
#include "Header.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

namespace astateful {
namespace mongo {
  bool Response::Accept( const char * data, int length ) {
    for ( int i = 0; i < length; i++ ) {
      m_buffer.push_back( data[i] );

      switch ( m_state ) {
      case state_e::SIZE:
        if ( m_buffer.size() == 4 ) {
          memcpy( &m_size, reinterpret_cast<char *>( &m_buffer[0] ), 4 );

          m_buffer.clear();
          m_state = state_e::HEADER;
        }
        break;
      case state_e::HEADER:
        if ( m_buffer.size() == 12 ) {
          m_header = std::make_unique<Header>( m_buffer );

          m_buffer.clear();
          m_state = state_e::FIELD;
        }
        break;
      case state_e::FIELD:
        if ( m_buffer.size() == 20 ) {
          memcpy( &m_flag, reinterpret_cast<const char *>( &m_buffer[0] ), 4 );
          memcpy( &m_id, reinterpret_cast<const char *>( &m_buffer[4] ), 8 );
          memcpy( &m_start, reinterpret_cast<const char *>( &m_buffer[12] ), 4 );
          memcpy( &m_count, reinterpret_cast<const char *>( &m_buffer[16] ), 4 );

          m_buffer.clear();
          m_state = state_e::BODY;
        }
        break;
      }
    }

    if ( m_size - ( 4 + 12 + 20 ) != m_buffer.size() ) return false;

    switch ( m_state ) {
    case state_e::BODY:
      if ( m_size - ( 4 + 12 + 20 ) == m_buffer.size() ) {
        return true;
      }
      break;
    }

    return false;
  }

  std::unique_ptr<Serialize> Response::GenerateSerialize( int32_t& position ) const {
    int32_t size;
    memcpy( &size, m_buffer.data() + position, 4 );
    std::vector<uint8_t> data( m_buffer.begin() + position,
                               m_buffer.begin() + position + size );

    position += size;

    return std::make_unique<Serialize>( data );
  }
}
}
