#include "astateful/bson/Element/Code.hpp"

#include <codecvt>

namespace astateful {
namespace bson {
  ElementCode::ElementCode( const std::string& key,
                            const std::wstring& data ) :
    Element( key ) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    m_data = convert.to_bytes( data );
  }

  ElementCode::ElementCode( const std::string& key,
                            const std::vector<uint8_t>& data ) :
    m_data( data.begin(), data.end() ),
    Element( key ) {}

  ElementCode::ElementCode( ElementCode&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementCode::operator std::wstring() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes( m_data );
  }

  std::wstring ElementCode::json( bool format,
                                  const std::wstring& tab,
                                  const std::wstring& indent,
                                  const int depth ) const {
    auto value = escapeJSON( *this );
    return ( format ) ? L"Code(\"" + value + L"\")" : L"\"" + value + L"\"";
  }

  std::vector<uint8_t> ElementCode::bson( error_e& error ) const {
    std::vector<uint8_t> output;
    if ( !appendString( error, output, m_data ) ) return {};
    return output;
  }
}
}
