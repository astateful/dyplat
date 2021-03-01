#include "autogen/token/graph/State/Start.hpp"

namespace astateful {
namespace token {
namespace graph {
  state_e StateStart::operator()( Context&, uint8_t ) {
    return state_e::Start;
  }
}
}
}
