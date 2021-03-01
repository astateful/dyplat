#include "astateful/plugin/Expand.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Iterate.hpp"
#include "astateful/bson/Element.hpp"
#include "astateful/algorithm/Context.hpp"

namespace astateful {
namespace plugin {
  using context_t = algorithm::Context<bson::Serialize, std::string>;

namespace {
  bool store_value( context_t& context,
                    const std::vector<uint8_t>& input,
                    const std::string& order ) {
    bson::Iterate iterate( input );
    std::string create;
    std::string index;

    while ( iterate ) {
      if ( iterate->key() == "create" ) {
        create = *iterate;

        if ( context.state.find( create ) == context.state.end() )
          return false;
      }

      if ( iterate->key() == "id" ) index = *iterate;

      ++iterate;
    }

    context.value[index] = { create, std::stoi( order ) };

    return true;
  }

  bool parse_value( context_t& context, const std::vector<uint8_t>& input,
                    bson::error_e& error ) {
    bson::Iterate iterate( input );

    while ( iterate ) {
      if ( iterate.enumId() == bson::element_e::OBJECT ) {
        auto bson = iterate->bson( error );
        if ( error != bson::error_e::CLEAN ) return false;

        if ( !store_value( context, bson, iterate->key() ) )
          return false;
      }

      ++iterate;
    }

    return true;
  }
}

  template <> bool Expand<bson::Serialize, std::string>::operator()(
    context_t& context,
    const std::unique_ptr<bson::Serialize>& serialize ) const {
    if ( !serialize ) return false;

    auto error = bson::error_e::CLEAN;

    std::vector<uint8_t> data( *serialize->at( "value" ) );

    if ( !parse_value( context, data, error ) ) return false;

    return true;
  }
}
}
