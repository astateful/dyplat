#include "astateful/protocol/ws/Request.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"
#include "astateful/algorithm/Context.hpp"

#include "State/Echo.hpp"

namespace astateful {
namespace protocol {
namespace ws {
  std::string Request<bson::Serialize, std::string>::hash() const {
    return m_ws.data;
  }

  void Request<bson::Serialize, std::string>::clear() {
    m_ws = { m_machine.initialState() };
  }

  bool Request<bson::Serialize, std::string>::initAlgorithm(
    algorithm::Context<bson::Serialize, std::string>& algorithm ) const {

    algorithm.state["echo"] = std::make_unique<StateEcho<bson::Serialize, std::string>>( m_ws.data );

    return true;
  }
}
}
}
