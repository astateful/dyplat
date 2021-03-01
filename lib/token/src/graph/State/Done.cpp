#include "autogen/token/graph/State/Done.hpp"

namespace astateful {
namespace token {
namespace graph {
  state_e StateDone::operator()( Context&, uint8_t ) {
    return state_e::Done;
  }
}
}
}
