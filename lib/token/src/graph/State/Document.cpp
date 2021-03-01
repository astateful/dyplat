#include "autogen/token/graph/State/Document.hpp"

#include "astateful/token/graph/Context.hpp"

namespace astateful {
namespace token {
namespace graph {
  state_e StateDocument::operator()( Context& context, uint8_t value ) {
    // Do not want to store the transition character.
    if ( value == '/' ) return state_e::Document;

    // Require specific length of 24 due to database requirements.
    context.document += value;
    if ( context.document.size() == 24 ) return state_e::Done;

    return state_e::Document;
  }
}
}
}
