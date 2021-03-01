#include "autogen/token/graph/State/Connection.hpp"

#include "astateful/token/graph/Context.hpp"

namespace astateful {
namespace token {
namespace graph {
  state_e StateConnection::operator()( Context& context, uint8_t value ) {
    // Do not want to store the transition character.
    if ( value == '/' ) return state_e::Connection;

    // Match against the explicit connections we would like to support.
    context.connection += value;
    if ( context.connection == "posts" ) return state_e::Done;

    return state_e::Connection;
  }
}
}
}
