#include "State/Version.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr <bson::Serialize> StateVersion <bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    std::unique_ptr<bson::Serialize> output( nullptr );

    return output;
  }
}
}
