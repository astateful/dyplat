#include "astateful/protocol/h1/Request.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"
#include "astateful/algorithm/Context.hpp"

#include "Domain/Echo.hpp"
#include "Domain/FS.hpp"
#include "Domain/Graph.hpp"
#include "Domain/Script.hpp"
#include "Domain/Token.hpp"

namespace astateful {
namespace protocol {
namespace h1 {
  std::string Request<bson::Serialize, std::string>::hash() const {
    return m_h1.rawId();
  }

  void Request<bson::Serialize, std::string>::clear() {
    m_h1 = { m_machine.initialState() };
  }

  bool Request<bson::Serialize, std::string>::initAlgorithm(
    algorithm::Context<bson::Serialize, std::string>& algorithm ) const {
    // The host header usually exists so if it does not we
    // throw an error since we cannot proceed without it.
    if ( !m_h1.headerAt( "Host" ) ) return false;

    const auto& host = m_h1.headerValue( "Host" );

    // The potential index of the executing state.
    std::string subdomain = "www";

    // Seems like this is the best way to validate that a subdomain
    // exists: we always need exactly two dots.
    size_t n_dots = std::count( host.begin(), host.end(), '.' );
    if ( n_dots == 2 ) {
      subdomain = host.substr( 0, host.find_first_of( "." ) );
    } else {
      return false;
    }

    // Instantiate the correct subdomain based on the host. Do not bother
    // trying to factor this code out since it has no utility in other protocols.
    if ( subdomain == "www" ) {
      DomainFS<bson::Serialize> domain( algorithm, m_public_html );
      return domain( m_h1 );
    } else if ( subdomain == "script") {
      DomainScript<bson::Serialize> domain( algorithm, m_script_path, m_memoize_path );
      return domain( m_h1 );
    } else if ( subdomain == "token" ) {
      DomainToken<bson::Serialize> domain( algorithm, m_mongo );
      return domain( m_h1 );
    } else if ( subdomain == "graph" ) {
      DomainGraph<bson::Serialize> domain( algorithm, m_mongo );
      return domain( m_h1 );
    } else if ( subdomain == "echo" ) {
      DomainEcho<bson::Serialize> domain( algorithm );
      return domain( m_h1 );
    }

    return false;
  }
}
}
}
