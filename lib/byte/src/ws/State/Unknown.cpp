#include "autogen/byte/ws/State/Unknown.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateUnknown::operator()( Context&, uint8_t ) {
    return state_e::Unknown;
  }
}
}
}