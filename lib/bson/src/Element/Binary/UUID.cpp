#include "astateful/bson/Element/Binary/UUID.hpp"

namespace astateful {
namespace bson {
  ElementBinaryUUID::ElementBinaryUUID(
    const std::string& key,
    const std::vector<uint8_t>& data ) :
    ElementBinary ( key, data ) {}

  ElementBinaryUUID::ElementBinaryUUID(
    const std::string& key,
    const std::string& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryUUID::ElementBinaryUUID( ElementBinaryUUID&& rhs ) :
    ElementBinary( std::move( rhs ) ) {}

  std::wstring ElementBinaryUUID::json( bool format,
                                        const std::wstring& tab,
                                        const std::wstring& indent,
                                        const int depth ) const {
    std::wstring value( *this );
    return ( format ) ? L"BinaryUUID(\"" + value + L"\")" : L"\"" + value + L"\"";
  }
}
}
