#include "Request/Delete.hpp"

namespace astateful {
namespace cache {
  RequestDelete::RequestDelete( const std::string& key, bool reply ) {
    m_buffer.push_back('d');
    m_buffer.push_back('e');
    m_buffer.push_back('l');
    m_buffer.push_back('e');
    m_buffer.push_back('t');
    m_buffer.push_back('e');
    m_buffer.push_back(' ');
    m_buffer.insert( m_buffer.end(), key.begin(), key.end() );

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
