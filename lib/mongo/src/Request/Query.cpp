#include "Request/Query.hpp"
#include "Header.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
  RequestQuery::RequestQuery( int option,
                              const std::string& ns,
                              int skip,
                              int limit,
                              const Serialize& query,
                              error_e& error,
                              Serialize * field ) :
    Request( 0, 0, op_e::QUERY, option, ns ) {
    appendInt( skip );
    appendInt( limit );
    assert( appendSerialize( query, error ) );

    // Somewhat ugly but place the burden on the class rather
    // than on the caller to provide a non empty field.
    if ( field == nullptr ) {
      assert( appendSerialize( {}, error ) );
    } else {
      assert( appendSerialize( *field, error ) );
    }

    populate();
  }
}
}
