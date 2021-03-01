#include "astateful/bson/Element/Timestamp.hpp"

namespace astateful {
namespace bson {
  ElementTimestamp::ElementTimestamp( const std::string& key,
                                      int32_t increment, int32_t second ) :
    m_increment( increment ),
    m_second( second ),
    Element( key ) {}

  ElementTimestamp::ElementTimestamp( ElementTimestamp&& rhs ) :
    Element( std::move( rhs ) ),
    m_increment( std::move( rhs.m_increment ) ),
    m_second( std::move( rhs.m_second ) ) {}

  ElementTimestamp::operator std::vector<uint8_t>() const {
    int32_t increment = m_increment;
    int32_t second = m_second;
    std::size_t size = sizeof( int32_t );
    uint8_t * data;

    std::vector<uint8_t> output;
    output.resize( size * 2 );

    data = reinterpret_cast<uint8_t *>( &increment );
    for ( std::size_t i = 0; i < size; ++i ) {
      output[i] = *data;
      data++;
    }

    data = reinterpret_cast<uint8_t *>( &second );
    for ( std::size_t i = size; i < size * 2; ++i ) {
      output[i] = *data;
      data++;
    }

    return output;
  }

  std::wstring ElementTimestamp::json( bool format,
                                       const std::wstring& tab,
                                       const std::wstring& indent,
                                       const int depth ) const {
    auto increment( std::to_wstring( m_increment ) );
    auto second( std::to_wstring( m_second ) );

    std::wstring output( L"" );
    output += ( format ) ? L"Timestamp({\n" : L"\n" + tab + L"{\n";
    output += tab + indent;
    output += L"\"increment\":";
    output += ( format ) ? L"Int(" + increment + L")" : increment;
    output += L",\n";
    output += tab + indent;
    output += L"\"seconds\":";
    output += ( format ) ? L"Int(" + second + L")" : second;
    output += L"\n";
    output += tab;
    output += ( format ) ? L"})" : L"}";

    return output;
  }

  std::vector<uint8_t> ElementTimestamp::bson( error_e& ) const {
    return *this;
  }
}
}
