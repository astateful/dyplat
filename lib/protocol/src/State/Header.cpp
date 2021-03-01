#include "State/Header.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateHeader<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux != "header" ) return nullptr;

    auto output = std::make_unique<bson::Serialize>();

    for ( const auto& header : m_h1.header )
      output->append( bson::ElementString( header.first, header.second ) );

    return output;
  }
}
}
