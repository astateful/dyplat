#include "Header.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
  Header::Header( int32_t id, int32_t response, op_e op ) :
    m_id( id ),
    m_response( response ),
    m_op( static_cast<int32_t>( op ) ) {}

  Header::Header( const std::vector<uint8_t>& input ) {
    memcpy( &m_id, &input[0], 4 );
    memcpy( &m_response, &input[4], 4 );
    memcpy( &m_op, &input[8], 4 );
  }

  std::vector<uint8_t> Header::pack( size_t size ) {
    std::vector<uint8_t> output;

    char * data = nullptr;

    assert( size < INT_MAX );

    data = reinterpret_cast<char *>( &size );
    for ( int i = 0; i < 4; ++i ) {
      output.push_back( *data );
      data++;
    }

    int32_t id = ( !m_id ) ? rand() : m_id;

    data = reinterpret_cast<char *>( &id );
    for ( int i = 0; i < 4; ++i ) {
      output.push_back( *data );
      data++;
    }

    data = reinterpret_cast<char *>( &m_response );
    for ( int i = 0; i < 4; ++i ) {
      output.push_back( *data );
      data++;
    }

    data = reinterpret_cast<char *>( &m_op );
    for ( int i = 0; i < 4; ++i ) {
      output.push_back( *data );
      data++;
    }

    return output;
  }
}
}
