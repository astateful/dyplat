#include "astateful/protocol/h2/Transform.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

namespace astateful {
namespace protocol {
namespace h2 {
  bool Transform<bson::Serialize, std::string>::operator()(
    const bson::Serialize& input,
    const std::string& exec,
    std::vector<uint8_t>& output ) const {
    std::vector<uint8_t> body( *input.at( "data" ) );

    output = body;

    return true;
  }
}
}
}
