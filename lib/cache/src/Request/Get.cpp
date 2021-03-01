#include "Request/Get.hpp"

namespace astateful {
namespace cache {
  RequestGet::RequestGet( const std::string& key, const std::string& type ) {
    m_buffer.insert( m_buffer.end(), type.begin(), type.end() );
    m_buffer.push_back( ' ' );
    m_buffer.insert( m_buffer.end(), key.begin(), key.end() );
    m_buffer.push_back( '\r' );
    m_buffer.push_back( '\n' );
  }
}
}
