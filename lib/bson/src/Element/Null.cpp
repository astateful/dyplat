#include "astateful/bson/Element/Null.hpp"

namespace astateful {
namespace bson {
  ElementNull::ElementNull( const std::string& key ) : Element( key ) {}

  ElementNull::ElementNull( ElementNull&& rhs ) :
    Element( std::move( rhs ) ) {}

  std::wstring ElementNull::json( bool format, const std::wstring& tab,
                                  const std::wstring& ident,
                                  const int depth ) const {
    return ( format ) ? L"Null(null)" : L"null";
  }
}
}
