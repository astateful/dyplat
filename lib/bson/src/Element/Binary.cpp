#include "astateful/bson/Element/Binary.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/crypto/base64.hpp"

namespace astateful {
namespace bson {
  ElementBinary::ElementBinary( const std::string& key,
                                const std::vector<uint8_t>& data ) :
    m_data( data.cbegin(), data.cend() ),
    Element( key ) {}

  ElementBinary::ElementBinary( const std::string& key,
                                const std::string& data ) :
    m_data( data.cbegin(), data.cend() ),
    Element( key ) {}

  ElementBinary::ElementBinary( const std::string& key,
                                const uint8_t * data,
                                const size_t size ) :
    Element( key ) {
    m_data.resize( size );
    memcpy( &m_data[0], data, size );
  }

  ElementBinary::ElementBinary( const std::string& key,
                                std::vector<uint8_t>::const_iterator begin,
                                std::vector<uint8_t>::const_iterator end ) :
  m_data( begin, end ),
  Element( key ) {}

  ElementBinary::ElementBinary( ElementBinary&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementBinary::operator std::wstring() const {
    auto base64_encode = crypto::base64encode( m_data );
    return std::wstring( base64_encode.begin(), base64_encode.end() );
  }

  ElementBinary::operator std::string() const {
    return crypto::base64encode( m_data );
  }

  std::vector<uint8_t> ElementBinary::bson( error_e& error ) const {
    std::vector<uint8_t> data( *this );
    size_t size( data.size() );

    std::vector<uint8_t> output;

    if ( !appendInt( output, size ) ) {
      error = error_e::TOO_LARGE;
      return {};
    }

    output.push_back( static_cast<char>( static_cast<sub_e>( *this ) ) );
    output.insert( output.end(), data.begin(), data.end() );

    return output;
  }
}
}
