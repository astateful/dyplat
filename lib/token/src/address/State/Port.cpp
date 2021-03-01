#include "autogen/token/address/State/Port.hpp"

#include "astateful/token/address/Context.hpp"

namespace astateful {
namespace token {
namespace address {
  state_e StatePort::operator()( Context& context, wchar_t value ) {
    // Do not want to store the transition character.
    if ( value == ':' ) return state_e::Port;

    context.port += value;

    return state_e::Done;
  }
}
}
}
