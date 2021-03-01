#include "autogen/token/address/State/Done.hpp"

#include "astateful/token/address/Context.hpp"

namespace astateful {
namespace token {
namespace address {
  state_e StateDone::operator()( Context& context, wchar_t value ) {
    context.port += value;

    return state_e::Done;
  }
}
}
}
