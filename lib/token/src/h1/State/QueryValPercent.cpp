#include "autogen/token/h1/State/QueryValPercent.hpp"

#include "astateful/token/h1/Context.hpp"

#include <sstream>

namespace astateful {
namespace token {
namespace h1 {
  state_e StateQueryValPercent::operator()( Context& context, uint8_t value ) {
    // Do not add the state transition character to the buffer.
    if ( value != '%' ) context.buffer += value;

    // We convert a pair of hexadecimal digits to the corresponding
    // ASCII value. The table takes care of ensuring digit validity.
    if ( context.buffer.size() == 2 ) {
      unsigned int x;

      std::stringstream ss;
      ss << std::hex << context.buffer;
      ss >> x;

      auto& val = context.query.back().second;
      val += static_cast<char>( x );

      context.buffer.clear();

      return state_e::QueryVal;
    }

    return state_e::QueryValPercent;
  }
}
}
}