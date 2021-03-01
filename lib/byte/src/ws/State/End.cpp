#include "autogen/byte/ws/State/End.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateEnd::operator()( Context&, uint8_t ) {
    return state_e::End;
  }
}
}
}