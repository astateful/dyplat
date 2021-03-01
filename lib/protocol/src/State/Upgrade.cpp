#include "State/Upgrade.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateUpgrade<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    const std::string& method = *( value.at( "method" )->at( "value" ) );
    auto error = bson::error_e::CLEAN;

    if ( flux == "output" ) {
      if ( method == "GET" ) {
        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementString( "version", L"HTTP/1.1" ) );
        output->append( bson::ElementInt( "code", 101 ) );
        output->append( bson::ElementString( "reason", L"Switching Protocols" ) );

        if ( !output->append( "header", *value.at( "ws" ), error ) )
          return nullptr;

        output->append( bson::ElementString( "body", "" ) );

        return output;
      }
    }

    return nullptr;
  }
}
}
