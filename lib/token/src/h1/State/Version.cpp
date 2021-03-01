#include "autogen/token/h1/State/Version.hpp"

#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateVersion::operator()( Context& context, uint8_t value ) {
    if ( value == ' ' ) {
      // Since we are always pushing one extra query pair, if we
      // are at the end of the query state, we remove the extra
      // pair only if its empty.
      const auto& pair = context.query.back();
      if ( pair.first == "" && pair.second == "" ) {
        context.query.pop_back();

        return state_e::Version;
      }
    }

    context.version += value;
    return state_e::Version;
  }
}
}
}