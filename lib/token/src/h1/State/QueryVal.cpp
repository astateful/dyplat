#include "autogen/token/h1/State/QueryVal.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateQueryVal::operator()( Context& context, uint8_t value ) {
    // Do not append the transition char to the query key.
    if ( value == '=' ) return state_e::QueryVal;

    context.query.back().second += value;

    return state_e::QueryVal;
  }
}
}
}