#include "State/Method.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateMethod<bson::Serialize, std::string>::operator () (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux != "method" ) { return nullptr; }

    if ( m_data != "OPTIONS" &&
          m_data != "GET" &&
          m_data != "HEAD" &&
          m_data != "POST" &&
          m_data != "PUT" &&
          m_data != "DELETE" &&
          m_data != "TRACE" &&
          m_data != "CONNECT" )
    {
      return nullptr;
    }

    auto output = std::make_unique<bson::Serialize>();
    output->append( bson::ElementString( "value", m_data ) );
    return output;
  }
}
}
