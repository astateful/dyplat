#include "autogen/token/h1/State/HeaderVal.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateHeaderVal::operator()( Context& context, uint8_t value ) {
    // Do not append the transition char to the header value.
    // Usually there is a leading space so we also do not care
    // about appending that either.
    auto& val = context.header.back().second;
    if ( value == ':' || ( value == ' ' && val.size() == 0 ) )
      return state_e::HeaderVal;

    val += value;

    return state_e::HeaderVal;
  }
}
}
}