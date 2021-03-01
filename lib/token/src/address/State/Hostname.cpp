#include "autogen/token/address/State/Hostname.hpp"

#include "astateful/token/address/Context.hpp"

namespace astateful {
namespace token {
namespace address {
  state_e StateHostname::operator()( Context& context, wchar_t value ) {
    context.hostname += value;

    return state_e::Hostname;
  }
}
}
}
