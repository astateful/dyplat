#include "autogen/token/h1/State/Done.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateDone::operator()( Context&, uint8_t ) {
    return state_e::Done;
  }
}
}
}