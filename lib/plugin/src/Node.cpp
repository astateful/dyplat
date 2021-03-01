#include "astateful/plugin/Node.hpp"

#include <cassert>

namespace astateful {
namespace plugin {
  mime_t generate_mime() {
    mime_t mime;

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"html" ),
                  std::forward_as_tuple( mime_e::HTML, "text/html" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"htm" ),
                  std::forward_as_tuple( mime_e::HTM, "text/html" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"css" ),
                  std::forward_as_tuple( mime_e::CSS, "text/css" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"js" ),
                  std::forward_as_tuple( mime_e::JS, "text/javascript" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"ico" ),
                  std::forward_as_tuple( mime_e::ICO, "image/x-icon" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"gif" ),
                  std::forward_as_tuple( mime_e::GIF, "image/gif" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"jpg" ),
                  std::forward_as_tuple( mime_e::JPG, "image/jpeg" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"jpeg" ),
                  std::forward_as_tuple( mime_e::JPEG, "image/jpeg" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"png" ),
                  std::forward_as_tuple( mime_e::PNG, "image/png" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"txt" ),
                  std::forward_as_tuple( mime_e::TXT, "text/plain" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"xml" ),
                  std::forward_as_tuple( mime_e::XML, "text/xml" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"json" ),
                  std::forward_as_tuple( mime_e::JSON, "text/json" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"pdf" ),
                  std::forward_as_tuple( mime_e::PDF, "application/pdf" ) );

    mime.emplace( std::piecewise_construct,
                  std::forward_as_tuple( L"bmp" ),
                  std::forward_as_tuple( mime_e::BMP, "image/bmp" ) );

    return mime;
  }

  mime_t Node::m_mime = generate_mime();

  Node::Node( const std::wstring& path ) :
    m_directory( false ),
    m_name( L"" ),
    m_extension( L"" ) {
    wchar_t name[50];

    if ( GetFileAttributesExW( path.c_str(), GetFileExInfoStandard,
      &m_attribute_data ) ) {
      m_directory = ( m_attribute_data.dwFileAttributes
                      & FILE_ATTRIBUTE_DIRECTORY ) ? true : false;

      if ( m_directory ) {
        wchar_t absolute[_MAX_PATH];

        assert( _wfullpath( absolute, path.c_str(), _MAX_PATH ) != nullptr );
        assert( _wsplitpath_s( absolute, nullptr, 0, nullptr, 0, name, 50,
          nullptr, 0 ) == 0 );
      } else {
        wchar_t extension[10];

        assert( _wsplitpath_s( path.c_str(), nullptr, 0, nullptr, 0, name,
          50, extension, 10 ) == 0 );

        if ( wcslen( extension ) > 0 ) {
          m_extension.assign( extension, wcslen( extension ) );
          m_extension = m_extension.substr( 1, m_extension.size() );
        }
      }

      m_name.assign( name, wcslen( name ) );
      m_path = path;
    }
  }

  int64_t Node::size() const {
    int64_t size = m_attribute_data.nFileSizeHigh;
    size <<= sizeof ( m_attribute_data.nFileSizeHigh ) * 8;
    size |= m_attribute_data.nFileSizeLow;

    return size;
  }

  bool Node::displayable() const {
    if ( m_directory ) return true;
    if ( m_mime.find( m_extension ) == m_mime.cend() )
      return false;

    return true;
  }

  const FILETIME& Node::created() const {
    return m_attribute_data.ftCreationTime;
  }

  const FILETIME& Node::accessed() const {
    return m_attribute_data.ftLastAccessTime;
  }

  const FILETIME& Node::altered() const {
    return m_attribute_data.ftLastWriteTime;
  }

  const std::string Node::contentType() const {
    if ( m_mime.find( m_extension ) == m_mime.cend() )
      return "";

    return m_mime.at( m_extension ).type();
  }

  bool Node::operator<( const Node& rhs ) const {
    if ( m_directory && !rhs.directory() ) return true;
    if ( !m_directory && rhs.directory() ) return false;

    return false;
  }
}
}
