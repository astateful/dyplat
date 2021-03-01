#include "astateful/bson/Element/Array.hpp"
#include "astateful/crypto/base64.hpp"

namespace astateful {
namespace bson {
  ElementArray::ElementArray( const std::string& key,
                              const std::vector<uint8_t>& data ) :
   m_data( data ),
   Element( key ) {}

  ElementArray::ElementArray( ElementArray&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementArray::operator std::wstring() const {
    auto base64_encode = crypto::base64encode( m_data );
    return std::wstring( base64_encode.begin(), base64_encode.end() );
  }

  ElementArray::operator std::string() const {
    return crypto::base64encode( m_data );
  }
}
}
