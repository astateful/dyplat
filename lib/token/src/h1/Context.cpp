#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace token {
namespace h1 {
  Context::Context( state_e state_ ) :
    version(),
    query(),
    method( "" ),
    uri( "" ),
    header(),
    state( state_ ),
    body(),
    buffer() {
    header.reserve( maxNumHeader );
    query.reserve( maxNumQuery );

    header.emplace_back( "", "" );
    query.emplace_back( "", "" );
  }

  std::string Context::rawId() const {
    std::string raw = method;
    raw += L' ';
    raw += uri;

    if ( query.size() > 0 ) {
      raw += '?';

      for ( const auto q : query ) {
        raw += q.first;
        raw += '=';
        raw += q.second;
        raw += '&';
      }

      raw.pop_back(); // Remove the extra '&'.
    }

    raw += ' ';
    raw += version;

    return raw;
  }

  bool Context::headerAt( const std::string& key ) const {
    for ( const auto& header : header )
      if ( header.first == key ) return true;

    return false;
  }

  const std::string& Context::headerValue( const std::string& key ) const {
    for ( const auto& header : header )
      if ( header.first == key ) return header.second;

    assert( false );
  }
}
}
}