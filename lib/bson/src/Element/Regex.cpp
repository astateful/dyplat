#include "astateful/bson/Element/Regex.hpp"

#include <codecvt>

namespace astateful {
namespace bson {
  ElementRegex::ElementRegex( const std::string& key,
                              std::wstring pattern,
                              std::wstring option ) :
    Element( key ) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

    m_pattern = convert.to_bytes( pattern );
    m_option = convert.to_bytes( option );
  }

  ElementRegex::ElementRegex( const std::string& key,
                              const std::vector <uint8_t>& pattern,
                              const std::vector <uint8_t>& option ) :
    m_pattern( pattern.begin(), pattern.end() ),
    m_option( option.begin(), option.end() ),
    Element( key ) {}

  ElementRegex::ElementRegex( ElementRegex&& rhs ) :
    Element( std::move( rhs ) ),
    m_pattern( std::move( rhs.m_pattern ) ),
    m_option( std::move( rhs.m_option ) ) {}

  std::wstring ElementRegex::json( bool format,
                                   const std::wstring& tab,
                                   const std::wstring& ident,
                                   const int depth ) const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    auto pattern( escapeJSON( convert.from_bytes( m_pattern ) ) );
    auto option( escapeJSON( convert.from_bytes( m_option ) ) );

    std::wstring output( L"" );
    output += ( format ) ? L"Regex({\n" : L"\n" + tab + L"{\n";
    output += tab + ident;
    output += L"\"pattern\":";
    output += ( format ) ? L"String(\"" + pattern + L"\")" : pattern;
    output += L",\n";
    output += tab + ident;
    output += L"\"option\":";
    output += ( format ) ? L"String(\"" + option + L"\")" : option;
    output += L"\n";
    output += tab;
    output += ( format ) ? L"})" : L"}";

    return output;
  }

  std::vector<uint8_t> ElementRegex::bson( error_e& error ) const {
    std::vector<uint8_t> output;

    if ( !validateString( error, m_pattern, false, false ) ) return {};
    if ( !validateString( error, m_option, false, false ) ) return {};

    output.insert( output.end(), m_pattern.begin(), m_pattern.end() );
    output.push_back( '\0' );
    output.insert( output.end(), m_option.begin(), m_option.end() );
    output.push_back( '\0' );

    return output;
  }
}
}
