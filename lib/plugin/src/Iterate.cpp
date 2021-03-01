#include "astateful/plugin/Iterate.hpp"
#include "astateful/plugin/Node.hpp"

#include <cassert>

namespace astateful {
namespace plugin {
  Iterate::Iterate( const std::wstring& path ) :
    m_handle( INVALID_HANDLE_VALUE ),
    m_path( path ),
    m_node( nullptr ) {
    auto attribute = GetFileAttributesW( m_path.c_str() );

    assert( ( attribute & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY );

    if ( attribute != INVALID_FILE_ATTRIBUTES ) {
      WIN32_FIND_DATAW data;

      std::wstring path( m_path );
      path += L"\\*";

      m_handle = FindFirstFileW( path.c_str(), &data );
      m_node = std::make_unique<Node>( m_path + L"\\" + data.cFileName );
    }
  }

  Iterate::Iterate( const Iterate& rhs ) :
    m_handle( rhs.m_handle ),
    m_path( rhs.m_path ),
    m_node( std::make_unique<Node>( *rhs.m_node ) ) {}

  Iterate& Iterate::operator=( const Iterate& rhs ) {
    if ( this != &rhs ) {
      m_handle = rhs.m_handle;
      m_path = rhs.m_path;
      m_node = std::make_unique<Node>( *rhs.m_node );
    }

    return *this;
  }

  Iterate& Iterate::operator++() {
    if ( m_handle == INVALID_HANDLE_VALUE ) return *this;

    WIN32_FIND_DATAW data;

    if ( !FindNextFileW( m_handle, &data ) ) {
      if ( FindClose( m_handle ) ) {
        m_handle = INVALID_HANDLE_VALUE;
      }
    } else {
      m_node = std::make_unique<Node>( m_path + L"\\" + data.cFileName );
    }

    return *this;
  }

  Iterate Iterate::operator++( int ) {
    Iterate iterate( *this );
    ++*this;

    return iterate;
  }

  Iterate::operator bool() const {
    return ( m_handle == INVALID_HANDLE_VALUE ) ? false : true;
  }
}
}
