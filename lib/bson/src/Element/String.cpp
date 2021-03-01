#include "astateful/bson/Element/String.hpp"

#include <codecvt>

namespace astateful {
namespace bson {
  ElementString::ElementString( const std::string& key,
                                const std::wstring& data ) :
    Element( key ) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    m_data = convert.to_bytes( data );
  }

  ElementString::ElementString( const std::string& key,
                                const std::string& data ) :
    m_data( data ),
    Element( key ) {}

  ElementString::ElementString( const std::string& key,
                                const std::vector<uint8_t>& data ) :
    m_data( data.begin (), data.end () ),
    Element( key ) {}

  ElementString::ElementString( ElementString&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementString::operator std::vector<uint8_t>() const {
    return { m_data.begin(), m_data.end() };
  }

  ElementString::operator std::wstring() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes( m_data );
  }

  std::wstring ElementString::json( bool format,
                                    const std::wstring& tab,
                                    const std::wstring& indent,
                                    const int depth ) const {
    auto value = escapeJSON( *this );
    return ( format ) ? L"String(\"" + value + L"\")" : L"\"" + value + L"\"";
  }

  std::vector<uint8_t> ElementString::bson( error_e& error ) const {
    std::vector<uint8_t> output;
    if ( !appendString( error, output, m_data ) ) return {};
    return output;
  }
}
}
