#include "autogen/token/h1/State/Method.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateMethod::operator()( Context& context, uint8_t value ) {
    context.method += value;
    return state_e::Method;
  }
}
}
}