#include "autogen/byte/ws/State/Mask.hpp"

#include "astateful/byte/ws/Context.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateMask::operator()( Context& context, uint8_t value ) {
    context.buffer.push_back( value );

    if ( context.buffer.size() != 4 ) return state_e::Mask;

    memcpy( context.mask, context.buffer.data(), context.buffer.size() );

    context.buffer.clear();

    return state_e::Data;
  }
}
}
}