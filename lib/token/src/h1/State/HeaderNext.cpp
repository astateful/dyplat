#include "autogen/token/h1/State/HeaderNext.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateHeaderNext::operator()( Context& context, uint8_t value ) {
    if ( context.header.size() < Context::maxNumHeader )
      // Prepare the next header pair.
      context.header.emplace_back( "", "" );

    return state_e::HeaderNext;
  }
}
}
}