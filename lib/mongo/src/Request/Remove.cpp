#include "Request/Remove.hpp"
#include "Header.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
  RequestRemove::RequestRemove( int option,
                                const std::string& ns,
                                int unknown,
                                int max_bson_size,
                                const Serialize& query,
                                error_e& error ) :
    Request( 0, 0, op_e::DEL, option, ns ) {
    appendInt( unknown );
    assert( setBody( query, max_bson_size, error ) );
    populate();
  }
}
}
