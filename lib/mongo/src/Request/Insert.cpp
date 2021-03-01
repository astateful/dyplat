#include "Request/Insert.hpp"
#include "Header.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
  RequestInsert::RequestInsert( int option,
                                const std::string& ns,
                                int max_bson_size,
                                const std::vector<bson::Serialize>& data,
                                error_e& error ) :
    Request( 0, 0, op_e::INSERT, option, ns ) {
    assert( setBody( data, max_bson_size, error ) );
    populate();
  }

  RequestInsert::RequestInsert( int option,
                                const std::string& ns,
                                int max_bson_size,
                                const bson::Serialize& data,
                                error_e& error ) :
    Request( 0, 0, op_e::INSERT, option, ns ) {
    assert( setBody( data, max_bson_size, error ) );
    populate();
  }
}
}
