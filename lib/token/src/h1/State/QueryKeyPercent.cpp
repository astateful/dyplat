#include "autogen/token/h1/State/QueryKeyPercent.hpp"

#include "astateful/token/h1/Context.hpp"

#include <sstream>

namespace astateful {
namespace token {
namespace h1 {
  state_e StateQueryKeyPercent::operator()( Context& context, uint8_t value ) {
    // Do not add the state transition character to the buffer.
    if ( value != '%' ) context.buffer += value;

    // We convert a pair of hexadecimal digits to the corresponding
    // ASCII value. The table takes care of ensuring digit validity.
    if ( context.buffer.size() == 2 ) {
      unsigned int x;

      std::stringstream ss;
      ss << std::hex << context.buffer;
      ss >> x;

      auto& key = context.query.back().first;
      key += static_cast<char>( x );

      context.buffer.clear();

      return state_e::QueryKey;
    }

    return state_e::QueryKeyPercent;
  }
}
}
}