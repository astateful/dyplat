#include "autogen/token/h1/State/Unknown.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateUnknown::operator()( Context&, uint8_t ) {
    return state_e::Unknown;
  }
}
}
}