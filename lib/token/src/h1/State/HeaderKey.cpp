#include "autogen/token/h1/State/HeaderKey.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateHeaderKey::operator()( Context& context, uint8_t value ) {
    context.header.back().first += value;

    return state_e::HeaderKey;
  }
}
}
}