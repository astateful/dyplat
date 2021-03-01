#include "astateful/bson/Element/Codescope.hpp"
#include "astateful/bson/Error.hpp"

namespace astateful {
namespace bson {
  ElementCodescope::ElementCodescope( const std::string& key,
                                      const std::vector<uint8_t>& code,
                                      const std::vector<uint8_t>& scope
                                    ) :
    m_code( code.begin(), code.end() ),
    m_scope( scope.cbegin(), scope.cend() ),
    Element( key ) {}

  ElementCodescope::ElementCodescope( ElementCodescope&& rhs ) :
    Element( std::move( rhs ) ),
    m_code( std::move( rhs.m_code ) ),
    m_scope( std::move( rhs.m_scope ) ) {}

  std::vector<uint8_t> ElementCodescope::bson( error_e& error ) const {
    std::vector<uint8_t> output;

    size_t ds, ts;
    ds = m_code.size() + 1;
    ts = 4 + 4 + ds + m_scope.size();

    if ( !appendInt( output, ts ) ) {
      error |= error_e::TOO_LARGE;
      return {};
    }

    if ( !appendInt( output, ds ) ) {
      error |= error_e::TOO_LARGE;
      return {};
    }

    output.insert( output.end(), m_code.begin(), m_code.end() );
    output.push_back( '\0' );
    output.insert( output.end(), m_scope.begin(), m_scope.end() );

    return output;
  }
}
}
