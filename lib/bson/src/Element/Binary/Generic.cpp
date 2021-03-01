#include "astateful/bson/Element/Binary/Generic.hpp"

namespace astateful {
namespace bson {
  ElementBinaryGeneric::ElementBinaryGeneric( const std::string& key,
    const std::vector<uint8_t>& data ) :
    ElementBinary( key, data ) {}

  ElementBinaryGeneric::ElementBinaryGeneric( const std::string& key,
    const std::string& data ) : ElementBinary( key, data ) {}

  ElementBinaryGeneric::ElementBinaryGeneric( const std::string& key,
    const uint8_t * data,
    const size_t size ) :
    ElementBinary( key, data, size ) {}

  ElementBinaryGeneric::ElementBinaryGeneric( const std::string& key,
    std::vector<uint8_t>::const_iterator begin,
    std::vector<uint8_t>::const_iterator end ) :
    ElementBinary( key, begin, end ) {}

  ElementBinaryGeneric::ElementBinaryGeneric( ElementBinaryGeneric&& rhs ) :
    ElementBinary( std::move( rhs ) ) {}

  std::wstring ElementBinaryGeneric::json( bool format,
                                           const std::wstring& tab,
                                           const std::wstring& indent,
                                           const int depth ) const {
    std::wstring value( *this );
    return ( format ) ? L"BinaryGeneric(\"" + value + L"\")" : L"\"" + value + L"\"";
  }
}
}
