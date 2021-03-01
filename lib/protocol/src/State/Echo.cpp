#include "State/Echo.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateEcho<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux == "output" ) {
      auto output = std::make_unique<bson::Serialize>();
      output->append( bson::ElementString( "body", m_data ) );

      return output;
    }

    return nullptr;
  }
}
}
