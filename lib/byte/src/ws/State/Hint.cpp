#include "autogen/byte/ws/State/Hint.hpp"

#include "astateful/byte/ws/Context.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateHint::operator()( Context& context, uint8_t value ) {
    context.length = value & 0x7F;

    return state_e::Length;
  }
}
}
}