#include "Request/More.hpp"
#include "Header.hpp"

namespace astateful {
namespace mongo {
  RequestMore::RequestMore( int option, const std::string& ns, int limit, uint64_t id ) :
    Request( 0, 0, op_e::GET_MORE, option, ns ) {
    appendInt( limit );
    appendLong( id );
    populate();
  }
}
}
