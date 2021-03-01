#include "autogen/byte/ws/State/Data.hpp"

#include "astateful/byte/ws/Context.hpp"

namespace astateful {
namespace byte {
namespace ws {
  state_e StateData::operator()( Context& context, uint8_t value ) {
    context.buffer.push_back( value );

    if ( context.buffer.size() != context.length ) return state_e::Data;

    context.data.resize( static_cast<size_t>( context.length ) );

    for ( int i = 0, j = 0; i < context.length; i++, j++ )
      context.data[j] = ( context.buffer[i] ^ context.mask[j % 4] );

    return state_e::End;
  }
}
}
}