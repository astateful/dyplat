#include "State/Negotiate.hpp"

#include "astateful/protocol/h2/Request.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"
#include "astateful/token/h1/Machine.hpp"
#include "astateful/algorithm/Context.hpp"

namespace astateful {
namespace protocol {
namespace h2 {
  std::string Request<bson::Serialize, std::string>::hash() const {
    return "";//m_h1.rawId();
  }

  void Request<bson::Serialize, std::string>::clear() {
    //m_h1 = { m_machine.initialState() };
  }

  bool Request<bson::Serialize, std::string>::initAlgorithm(
    algorithm::Context<bson::Serialize, std::string>& algorithm ) const {

    algorithm.exec = "bla";

    algorithm.state["bla"] = std::make_unique<StateNegotiate<bson::Serialize, std::string>>( m_h2 );

    return true;
  }
}
}
}
