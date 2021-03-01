#include "astateful/bson/Element/Int.hpp"

#include <cassert>

namespace astateful {
namespace bson {
  ElementInt::ElementInt( const std::string& key, const int32_t& data ) :
    m_data( data ),
    Element( key ) {}

  ElementInt::ElementInt( const std::string& key, const size_t& data ) :
    Element( key ) {
    assert( data <= INT_MAX );
    m_data = static_cast<int32_t>( data );
  }

  ElementInt::ElementInt( const std::string& key, const std::string& data ) :
    m_data( std::stoi( data ) ),
    Element( key ) {}

  ElementInt::ElementInt( ElementInt&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementInt::operator std::vector<uint8_t>() const {
    std::vector<uint8_t> output( 4 );

    for ( int i = 0; i < output.size(); ++i )
      output[i] = ( m_data >> ( i * 8 ) );

    return output;
  }

  ElementInt::operator std::wstring() const {
    return std::to_wstring( m_data );
  }

  ElementInt::operator std::string() const {
    return std::to_string( m_data );
  }

  std::wstring ElementInt::json( bool format,
                                 const std::wstring& tab,
                                 const std::wstring& indent,
                                 const int depth ) const {
    auto value = escapeJSON( *this );
    return ( format ) ? L"Int(" + value + L")" : value;
  }

  std::vector<uint8_t> ElementInt::bson( error_e& ) const {
    return *this;
  }
}
}
