#include "astateful/bson/Element/Binary/Function.hpp"

namespace astateful {
namespace bson {
  ElementBinaryFunction::ElementBinaryFunction(
    const std::string& key,
    const std::vector<uint8_t>& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryFunction::ElementBinaryFunction(
    const std::string& key,
    const std::string& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryFunction::ElementBinaryFunction( ElementBinaryFunction&& rhs ) :
    ElementBinary( std::move( rhs ) ) {}

  std::wstring ElementBinaryFunction::json( bool format,
                                            const std::wstring& tab,
                                            const std::wstring& indent,
                                            const int depth ) const {
    std::wstring value( *this );
    return ( format ) ? L"BinaryFunction(\"" + value + L"\")" : L"\"" + value + L"\"";
  }
}
}
