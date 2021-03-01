#include "astateful/bson/Element/Bool.hpp"

namespace astateful {
namespace bson {
  ElementBool::ElementBool( const std::string& key, bool data ) :
    m_data( data ),
    Element( key ) {}

  ElementBool::ElementBool( ElementBool&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementBool::operator std::wstring() const {
    return ( m_data ) ? L"true" : L"false";
  }

  ElementBool::operator std::string() const {
    return ( m_data ) ? "true" : "false";
  }

  ElementBool::operator int32_t() const {
    return ( m_data ) ? 1 : 0;
  }

  std::wstring ElementBool::json( bool format,
                                  const std::wstring& tab,
                                  const std::wstring& indent,
                                  const int depth ) const {
    auto value = escapeJSON( *this );
    return ( format ) ? L"Boolean(" + value + L")" : value;
  }

  std::vector<uint8_t> ElementBool::bson( error_e& ) const {
    auto i = int32_t( *this );

    std::vector<uint8_t> data;
    data.push_back( i != 0 );

    return data;
  }
}
}
