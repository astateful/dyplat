#include "autogen/token/h1/State/Body.hpp"

#include "astateful/token/h1/Context.hpp"

#include <string>

namespace astateful {
namespace token {
namespace h1 {
  state_e StateBody::operator()( Context& context, uint8_t value ) {
    const auto& content_length = context.headerValue( "Content-Length" );

    context.body += value;

    if ( context.body.size() == std::stoi( content_length ) )
      return state_e::Done;

    return state_e::Body;
  }
}
}
}