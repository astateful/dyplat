#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Iterate.hpp"
#include "astateful/bson/Element/Codescope.hpp"
#include "astateful/bson/Element/Array.hpp"
#include "astateful/bson/Element/Null.hpp"
#include "astateful/bson/Element/Object.hpp"
#include "astateful/bson/Error.hpp"

#include <algorithm>
#include <codecvt>
#include <cassert>

namespace astateful {
namespace bson {
  Serialize::Serialize( const std::vector<uint8_t>& data ) {
    Iterate iterate( data );
    while ( iterate ) {
      m_element.push_back( std::move( iterate.current() ) );

      ++iterate;
    }
  }

  Serialize::Serialize( Serialize&& rhs ) :
    m_element( std::move( rhs.m_element ) ) {}

  Serialize& Serialize::operator=( Serialize && rhs ) {
    if ( this != &rhs ) {
      m_element = std::move( rhs.m_element );
    }

    return *this;
  }

  bool Serialize::append( const std::string& key, const Serialize& input,
                          error_e& error ) {
    auto element = std::make_unique<ElementObject>( key, input, error );

    if ( ( error & error_e::NOT_UTF8 ) == error_e::NOT_UTF8 )
      return false;

    m_element.push_back( std::move( element ) );

    return true;
  }

  bool Serialize::appendArray( const std::string& key, const Serialize& input,
                               error_e& error ) {
    auto element = std::make_unique<ElementArray>( key, input, error );

    if ( ( error & error_e::NOT_UTF8 ) == error_e::NOT_UTF8 )
      return false;

    m_element.push_back( std::move( element ) );

    return true;
  }

  bool Serialize::append( const std::string& key,
                          const std::map<int, Serialize>& input,
                          error_e& error ) {
    Serialize compiled;

    for ( const auto& i : input )
      if ( !compiled.append( std::to_string( i.first ), i.second, error ) )
        return false;

    auto element = std::make_unique<ElementArray>( key, compiled, error );

    if ( ( error & error_e::NOT_UTF8 ) == error_e::NOT_UTF8 )
      return false;

    m_element.push_back( std::move( element ) );

    return true;
  }

  bool Serialize::bson( std::vector<uint8_t>& output, error_e& error ) const {
    output.push_back( '\0' );
    output.push_back( '\0' );
    output.push_back( '\0' );
    output.push_back( '\0' );

    for ( const auto& element : m_element ) {
      const auto& key = element->key();

      // Start an element at this point so that we take into account
      // the NULL element (we do not add data for this element).
      output.push_back( static_cast<char>( static_cast<element_e>( *element ) ) );
      output.insert( output.end(), key.begin(), key.end() );
      output.push_back( '\0' );

      if ( *element == element_e::EMPTY ) continue;

      const auto bson = element->bson( error );

      output.insert( output.end(), bson.begin(), bson.end() );
    }

    if ( ( error & error_e::NOT_UTF8 ) == error_e::NOT_UTF8 )
      return false;

    // The specification requires that we end the document
    // with a null terminating character.
    output.push_back( '\0' );

    // The BSON specification requires that the size of the
    // serialization object fits into 4 bytes...
    if ( output.size() > INT_MAX ) return false;

    auto size = static_cast<int32_t>( output.size() );
    auto data = reinterpret_cast<char *>( &size );

    for ( int i = 0; i < 4; ++i ) {
      output[i] = *data;
      data++;
    }

    return true;
  }

  Element * Serialize::at( const std::string& key ) const {
    auto it = std::find_if( m_element.begin(), m_element.end(), [&key]
                            ( const std::unique_ptr<Element>& element ) {
      return ( element->key() == key );
    } );

    if ( it == end( m_element ) ) return nullptr;

    return it->get();
  }

  Element& Serialize::operator[]( const char * key ) {
    auto it = std::find_if( m_element.begin(), m_element.end(), [&key]
                            ( const std::unique_ptr<Element>& element ) {
      return ( element->key() == std::string( key ) );
    } );

    if ( it == end( m_element ) ) {
      m_element.push_back( std::make_unique<ElementNull>( key ) );
      return *m_element.back();
    }

    return *(*it);
  }

  std::wstring Serialize::json( int depth, bool format ) const {
    std::wstring tab( L"" );
    std::wstring indent( L"   " );
    std::wstring output( L"" );
    int count = 0;

    for ( int i = 0; i <= depth; ++i ) tab += indent;

    if ( depth == 0 ) output += L"{\n";

    for ( const auto& element : m_element ) {
      output += tab;
      output += L"\"" + element->escapeJSON( element->wideKey() ) + L"\":";
      output += element->json( format, tab, indent, depth );
      output += L",\n";
      count++;
    }

    if ( count > 0 ) {
      output = output.substr( 0, output.size () - 2 );
      output += L"\n";
    }

    if ( depth == 0 ) output += L"}";

    return output;
  }

  Serialize::~Serialize() {}

  ElementCodescope::ElementCodescope( const std::string& key,
                                      const std::string& code,
                                      const Serialize& scope,
                                      error_e& error ) :
    m_code( code ),
    Element( key ) {
    assert( scope.bson( m_scope, error ) );
  }

  ElementCodescope::ElementCodescope( const std::string& key,
                                      const std::wstring& code,
                                      const Serialize& scope,
                                      error_e& error ) :
    Element( key ) {
    std::wstring_convert<std::codecvt_utf8 <wchar_t>> convert;
    m_code = convert.to_bytes( code );
    assert( scope.bson( m_scope, error ) );
  }

  ElementArray::ElementArray( const std::string& key,
                              const Serialize& data,
                              error_e& error ) :
    Element( key ) {
    assert( data.bson( m_data, error ));
  }

  ElementObject::ElementObject( const std::string& key,
                                const Serialize& data,
                                error_e& error ) :
    Element( key ) {
    assert( data.bson( m_data, error ));
  }

  std::wstring ElementCodescope::json( bool format,
                                       const std::wstring& tab,
                                       const std::wstring& indent,
                                       const int depth ) const {
    std::wstring output( L"" );
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    auto code = escapeJSON( convert.from_bytes( m_code ) );
    Serialize scope( m_scope );

    output += ( format ) ? L"CWS({\n" : L"\n" + tab + L"{\n";
    output += tab + indent;
    output += L"\"code\":";
    output += ( format ) ? L"Code(\"" + code + L"\")" : L"\"" + code + L"\"";
    output += L",\n";
    output += tab + indent;
    output += L"\"scope\":\n";

    for ( int i = 0; i < depth; ++i ) output += tab;

    output += indent;
    output += L"{\n" + scope.json( depth + 2, format );

    for ( int i = 0; i < depth; ++i ) output += tab;

    output += indent;
    output += L"}";
    output += ( format ) ? L"\n" + tab + L"})" : L"\n" + tab + L"}";

    return output;
  }

  std::wstring ElementArray::json( bool format,
                                   const std::wstring& tab,
                                   const std::wstring& indent,
                                   const int depth ) const {
    std::wstring output( L"" );
    output += L"\n" + tab + L"[\n";
    output += Serialize( m_data ).json( depth + 1, format );
    output += tab + L"]";

    return output;
  }

  std::wstring ElementObject::json( bool format,
                                    const std::wstring& tab,
                                    const std::wstring& indent,
                                    const int depth ) const {
    std::wstring output( L"" );
    output += L"\n" + tab + L"{\n";
    output += Serialize( m_data ).json( depth + 1, format );
    output += tab + L"}";

    return output;
  }

  std::wostream& operator<<( std::wostream& stream, const Serialize& input ) {
    stream << input.json( 0, false );

    return stream;
  }
}
}
