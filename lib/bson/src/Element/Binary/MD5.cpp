#include "astateful/bson/Element/Binary/MD5.hpp"

namespace astateful {
namespace bson {
  ElementBinaryMD5::ElementBinaryMD5(
    const std::string& key,
    const std::vector<uint8_t>& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryMD5::ElementBinaryMD5(
    const std::string& key,
    const std::string& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryMD5::ElementBinaryMD5( ElementBinaryMD5&& rhs ) :
    ElementBinary( std::move( rhs ) ) {}

  std::wstring ElementBinaryMD5::json( bool format,
                                       const std::wstring& tab,
                                       const std::wstring& indent,
                                       const int depth ) const {
    std::wstring value( *this );
    return ( format ) ? L"BinaryMD5(\"" + value + L"\")" : L"\"" + value + L"\"";
  }
}
}
