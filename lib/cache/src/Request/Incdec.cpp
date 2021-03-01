#include "Request/Incdec.hpp"

#include <string>

namespace astateful {
namespace cache {
  RequestIncdec::RequestIncdec( const std::string& key,
                                const std::string& type,
                                uint64_t diff,
                                bool reply ) {
    const std::string str_diff = std::to_string( diff );

    m_buffer.insert( m_buffer.end(), type.begin(), type.end() );
    m_buffer.push_back( ' ' );
    m_buffer.insert( m_buffer.end(), key.begin(), key.end() );
    m_buffer.push_back( ' ' );
    m_buffer.insert( m_buffer.end(), str_diff.begin(), str_diff.end() );

    if ( !reply ) {
      m_buffer.push_back( ' ' );
      m_buffer.push_back( 'n' );
      m_buffer.push_back( 'o' );
      m_buffer.push_back( 'r' );
      m_buffer.push_back( 'e' );
      m_buffer.push_back( 'p' );
      m_buffer.push_back( 'l' );
      m_buffer.push_back( 'y' );
    }

    m_buffer.push_back( '\r' );
    m_buffer.push_back( '\n' );
  }
}
}
