#include "astateful/bson/Element/Binary/UDEF.hpp"

namespace astateful {
namespace bson {
  ElementBinaryUDEF::ElementBinaryUDEF (
    const std::string& key,
    const std::vector<uint8_t>& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryUDEF::ElementBinaryUDEF(
    const std::string& key,
    const std::string& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryUDEF::ElementBinaryUDEF( ElementBinaryUDEF&& rhs ) :
    ElementBinary( std::move( rhs ) ) {}

  std::wstring ElementBinaryUDEF::json( bool format,
                                        const std::wstring& tab,
                                        const std::wstring& indent,
                                        const int depth ) const {
    std::wstring value( *this );
    return ( format ) ? L"BinaryUDEF(\"" + value + L"\")" : L"\"" + value + L"\"";
  }
}
}
