#include "astateful/byte/ws/Context.hpp"

namespace astateful {
namespace byte {
namespace ws {
  Context::Context( state_e state ) :
    state( state ) {
    memset( mask, 0, 4 );
  }
}
}
}