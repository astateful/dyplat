#include "autogen/byte/ws/State/Length.hpp"

#include "astateful/byte/ws/Context.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateLength::operator()( Context& context, uint8_t value ) {
    if ( context.length <= 125 ) {
      context.buffer.push_back( value );
      return state_e::Mask;
    } else if ( context.length == 126 ) {
      context.buffer.push_back( value );

      if ( context.buffer.size() != 2 ) return state_e::Length;

      std::vector<uint8_t> reverse;
      reverse.push_back( context.buffer[1] );
      reverse.push_back( context.buffer[0] );

      uint32_t length = 0;
      memcpy( &length, reverse.data(), reverse.size() );

      context.length = length;
      context.buffer.clear();
      return state_e::Mask;
    } else if ( context.length == 127 ) {
      context.buffer.push_back( value );

      if ( context.buffer.size() != 8 ) return state_e::Length;

      std::vector<uint8_t> reverse;
      reverse.push_back( context.buffer[7] );
      reverse.push_back( context.buffer[6] );
      reverse.push_back( context.buffer[5] );
      reverse.push_back( context.buffer[4] );
      reverse.push_back( context.buffer[3] );
      reverse.push_back( context.buffer[2] );
      reverse.push_back( context.buffer[1] );
      reverse.push_back( context.buffer[0] );

      uint64_t length = 0;
      memcpy( &length, reverse.data(), reverse.size() );

      context.length = length;
      context.buffer.clear();
      return state_e::Mask;
    }
  }
}
}
}