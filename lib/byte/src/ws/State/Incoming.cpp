#include "autogen/byte/ws/State/Incoming.hpp"

#include "astateful/byte/ws/Context.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateIncoming::operator()( Context& context, uint8_t value ) {
    auto fin = ( value >> 1 ) & 1;
    auto rsv1 = ( value >> 2 ) & 1;
    auto rsv2 = ( value >> 3 ) & 1;
    auto rsv3 = ( value >> 4 ) & 1;

    context.finished = ( fin == 1 ) ? true : false;

    return state_e::Hint;
  }
}
}
}