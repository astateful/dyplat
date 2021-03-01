#include "State/Query.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/token/h1/Context.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateQuery<bson::Serialize, std::string>::operator()(
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux != "query" ) { return nullptr; }

    if ( m_h1.query.size() == 0 ) { return nullptr; }

    auto output = std::make_unique<bson::Serialize>();

    for ( const auto& query : m_h1.query )
      output->append( bson::ElementString { query.first, query.second } );

    return output;
  }
}
}
