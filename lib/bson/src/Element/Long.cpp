#include "astateful/bson/Element/Long.hpp"

namespace astateful {
namespace bson {
  ElementLong::ElementLong( const std::string& key, const int64_t& data ) :
    Element( key ),
    m_data( data ) {}

  ElementLong::ElementLong( ElementLong&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementLong::operator std::vector<uint8_t>() const {
    int64_t temporary = m_data;
    std::size_t size = sizeof ( int64_t );

    std::vector<uint8_t> output;
    output.resize( size );

    auto data = reinterpret_cast<uint8_t *>( &temporary );
    for ( std::size_t i = 0; i < size; ++i ) {
      output[i] = *data;
      data++;
    }

    return output;
  }

  ElementLong::operator std::wstring() const {
    return std::to_wstring( m_data );
  }

  ElementLong::operator std::string() const {
    return std::to_string( m_data );
  }

  std::wstring ElementLong::json( bool format,
                                  const std::wstring& tab,
                                  const std::wstring& indent,
                                  const int depth ) const {
    auto value = escapeJSON( *this );
    return ( format ) ? L"Long(" + value + L")" : value;
  }

  std::vector<uint8_t> ElementLong::bson( error_e& ) const {
    return *this;
  }
}
}
