#include "astateful/plugin/Compress.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/algorithm/Context.hpp"

namespace astateful {
namespace plugin {
  const std::unique_ptr<bson::Serialize> Compress<bson::Serialize, std::string>::operator() (
    const algorithm::Context<bson::Serialize, std::string>& context ) const {
    // Do not use the default value map since its unordered. We need to
    // preserve the order so that when the parse evaluator reads the values
    // back, they are evaluated in the right order.
    std::map<int, bson::Serialize> values;

    // TODO: Propogate any BSON errors to the caller.
    auto error = bson::error_e::CLEAN;

    for ( const auto& value : context.value ) {
      // The key for the map is the order in which the value was generated
      // This will do the sorting for us, so when we read the serialization
      // back, the value map will be ready.
      const auto& key = value.second.order();

      values[key].append( bson::ElementString( "id", value.first ) );
      values[key].append( bson::ElementString( "create", value.second.create() ) );
    }

    auto output = std::make_unique<bson::Serialize>();
    if ( !output->append( "value", values, error ) ) return nullptr;

    return output;
  }
}
}
