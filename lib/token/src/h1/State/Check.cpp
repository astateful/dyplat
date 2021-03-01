#include "autogen/token/h1/State/Check.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateCheck::operator()( Context& context, uint8_t value ) {
    // We always push one too many header values, so prune the last
    // one only if its empty (it is probably always empty anyway).
    auto& pair = context.header.back();
    if ( pair.first == "" && pair.second == "" ) context.header.pop_back();

    if ( context.headerAt( "Content-Length" ) ) return state_e::Body;

    return state_e::Done;
  }
}
}
}