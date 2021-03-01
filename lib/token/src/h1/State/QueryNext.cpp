#include "autogen/token/h1/State/QueryNext.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateQueryNext::operator()( Context& context, uint8_t ) {
    if ( context.query.size() < Context::maxNumQuery )
      // Prepare the next query pair.
      context.query.emplace_back( "", "" );

    return state_e::QueryNext;
  }
}
}
}