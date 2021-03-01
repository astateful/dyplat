#include "autogen/token/h1/State/URI.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateUri::operator()( Context& context, uint8_t value ) {
    if ( value == ' ' ) return state_e::Uri;

    context.uri += value;
    return state_e::Uri;
  }
}
}
}