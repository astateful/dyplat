#include "astateful/bson/Element.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/ObjectId.hpp"

#include <codecvt>
#include <sstream>
#include <iomanip>
#include <cassert>

namespace astateful {
namespace bson {
namespace {
  //! See http://www.zedwood.com/article/cpp-is-valid-utf8-string-function
  bool utf8_check_is_valid( const std::string& string ) {
    int c,n,j;
    size_t i,ix;
    for (i=0, ix=string.length(); i < ix; i++) {
        c = (unsigned char) string[i];
        //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
        if (0x00 <= c && c <= 0x7f) n=0; // 0bbbbbbb
        else if ((c & 0xE0) == 0xC0) n=1; // 110bbbbb
        else if ( c==0xed && i<(ix-1) && ((unsigned char)string[i+1] & 0xa0)==0xa0) return false; //U+d800 to U+dfff
        else if ((c & 0xF0) == 0xE0) n=2; // 1110bbbb
        else if ((c & 0xF8) == 0xF0) n=3; // 11110bbb
        //else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        //else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return false;
        for (j=0; j<n && i<ix; j++) { // n bytes matching 10bbbbbb follow ?
            if ((++i == ix) || (( (unsigned char)string[i] & 0xC0) != 0x80))
                return false;
        }
    }
    return true;
  }

  bool is_string_dbref( const std::string& input ) {
    if ( input.length() >= 4 ) {
      if ( input[1] == 'r' && input[2] == 'e' && input[3] == 'f' ) {
        return true;
      }
    } else if ( input.length() >= 3 ) {
      if ( input[1] == 'i' && input[2] == 'd' ) {
        return true;
      } else if ( input[1] == 'd' && input[2] == 'b' ) {
        return true;
      }
    }

    return false;
  }
}

  Element::Element( const std::string& key ) : m_key( key ) {}

  Element::Element( Element&& rhs ) :
    m_key( std::move( rhs.m_key ) ) {}

  const std::wstring Element::wideKey() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

    return convert.from_bytes( m_key );
  }

  Element::operator bool() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator int32_t() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator int64_t() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator double() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator std::vector<uint8_t>() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator std::wstring() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator std::string() const {
    assert( false && "not implemented" );
    return {};
  }

  Element::operator ObjectId() const {
    assert( false && "not implemented" );
    return {};
  }

  bool Element::appendInt( std::vector<uint8_t>& output, size_t size ) {
    if ( size > INT_MAX ) return false;

    auto data = reinterpret_cast<int32_t*>( &size );
    auto buffer = reinterpret_cast<char *>( data );
    for ( int i = 0; i < 4; ++i ) {
      output.push_back( *buffer );
      buffer++;
    }

    return true;
  }

  bool Element::appendString( error_e& error, std::vector<uint8_t>& output,
                              const std::string& value ) {
    size_t length = value.length() + 1;

    if ( !validateString( error, value, false, false ) ) return false;
    if ( !appendInt( output, length ) ) return false;

    output.insert( output.end(), value.begin(), value.end() );
    output.push_back( '\0' );

    return true;
  }

  std::wstring Element::escapeJSON( const std::wstring& input ) {
    std::wstringstream output;

    for ( const auto& i : input ) {
      const auto u = unsigned( i );
      if ( u < '\x20' || u > '\x7F' || i == '\\' || i == '"' ) {
        output << "\\u";
        output << std::setfill( L'0' );
        output << std::setw( 4 );
        output << std::hex;
        output << u;
      } else { output << i; }
    }

    return output.str();
  }
    
  bool Element::validateString( error_e& error, const std::string& value,
                                bool check_dot, bool check_dollar ) {
    if ( check_dollar && value[0] == '$' ) {
      if ( !is_string_dbref( value ) ) {
        error |= error_e::FIELD_INIT_DOLLAR;
      }
    }

    size_t n_dots = std::count( value.begin(), value.end(), '.' );
    if ( n_dots > 0 ) error |= error_e::FIELD_HAS_DOT;

    if ( !utf8_check_is_valid( value ) ) {
      error |= error_e::NOT_UTF8;
      return false;
    }

    return true;
  }
}
}
