#include "Request/Update.hpp"
#include "Header.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
  RequestUpdate::RequestUpdate( int option,
                                const std::string& ns,
                                update_e update,
                                int max_bson_size,
                                const Serialize& condition,
                                const Serialize& data,
                                error_e& error ) :
    Request( 0, 0, op_e::UPDATE, option, ns ) {
    appendInt( static_cast<int>( update ) );
    assert( appendSerialize( condition, error ) );
    assert( setBody( data, max_bson_size, error ) );
    populate();
  }
}
}
