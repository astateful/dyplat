#include "astateful/bson/Element/Double.hpp"

namespace astateful {
namespace bson {
  ElementDouble::ElementDouble( const std::string& key, double data ) :
    m_data( data ),
    Element( key ) {}

  ElementDouble::ElementDouble( const std::string& key,
                                const std::string& data ) :
    m_data( std::stod( data ) ),
    Element( key ) {}

  ElementDouble::ElementDouble( ElementDouble&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementDouble::operator std::string() const {
    return std::to_string( m_data );
  }

  ElementDouble::operator std::vector<uint8_t>() const {
    double temporary = m_data;
    std::vector<uint8_t> output( sizeof( double ) );

    auto data = reinterpret_cast<uint8_t *>( &temporary );
    for ( std::size_t i = 0; i < output.size(); ++i ) {
      output[i] = *data;
      data++;
    }

    return output;
  }

  ElementDouble::operator std::wstring() const {
    return std::to_wstring( m_data );
  }

  std::wstring ElementDouble::json( bool format,
                                    const std::wstring& tab,
                                    const std::wstring& indent,
                                    const int depth ) const {
    auto value = escapeJSON( std::to_wstring( m_data ) );
    return ( format ) ? L"Double(" + value + L")" : value;
  }

  std::vector<uint8_t> ElementDouble::bson( error_e& ) const {
    return *this;
  }
}
}
