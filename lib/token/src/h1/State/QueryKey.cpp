#include "autogen/token/h1/State/QueryKey.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateQueryKey::operator()( Context& context, uint8_t value ) {
    // Do not append the transition char to the query key.
    if ( value == '?' ) return state_e::QueryKey;

    context.query.back().first += value;

    return state_e::QueryKey;
  }
}
}
}