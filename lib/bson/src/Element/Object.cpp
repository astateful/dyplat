#include "astateful/bson/Element/Object.hpp"
#include "astateful/crypto/base64.hpp"

namespace astateful {
namespace bson {
  ElementObject::ElementObject( const std::string& key,
                                const std::vector<uint8_t>& data ) :
    m_data( data.cbegin(), data.cend() ),
    Element( key ) {}

  ElementObject::ElementObject( ElementObject&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementObject::operator std::wstring() const {
    auto base64_encode = crypto::base64encode( m_data );
    return std::wstring( base64_encode.begin(), base64_encode.end() );
  }

  ElementObject::operator std::string() const {
    return crypto::base64encode( m_data );
  }
}
}
