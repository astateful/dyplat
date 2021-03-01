#include "Request/Kill.hpp"
#include "Header.hpp"

namespace astateful {
namespace mongo {
  RequestKill::RequestKill( int option, int unknown, uint64_t id ) :
    Request( 0, 0, op_e::KILL_CURSORS, option ) {
    appendInt( unknown );
    appendLong( id );
    populate();
  }
}
}
