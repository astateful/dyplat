#include "astateful/bson/Element/ObjectId.hpp"

namespace astateful {
namespace bson {
  ElementObjectId::ElementObjectId( const std::string& key ) :
    Element ( key ),
    m_data() {}

  ElementObjectId::ElementObjectId( const std::string& key,
                                    const std::string& data ) :
    Element( key ),
    m_data( data) {}

  ElementObjectId::ElementObjectId( const std::string& key,
                                    const std::vector<uint8_t>& data ) :
    m_data( data ),
    Element( key ) {}

  ElementObjectId::ElementObjectId( const std::string& key,
                                    const ObjectId& data ) :
    Element( key ),
    m_data( data) {}

  ElementObjectId::ElementObjectId( ElementObjectId&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementObjectId::operator std::wstring() const {
    std::string multi_byte = *this;
    return std::wstring( multi_byte.begin(), multi_byte.end() );
  }

  std::wstring ElementObjectId::json( bool format, const std::wstring& tab,
                                      const std::wstring& indent,
                                      const int depth ) const {
    std::wstring value = *this;
    return ( format ) ? L"ObjectId(\"" + value + L"\")" : L"\"" + value + L"\"";
  }
}
}
