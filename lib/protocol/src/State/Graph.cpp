#include "State/Graph.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateGraph<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    const std::string& method = *( value.at( "method" )->at( "value" ) );

    if ( flux == "output" ) {
      if ( method == "GET" || method == "POST" || method == "PUT" || method == "DELETE"  ) {
        bson::Serialize join;
        auto error = bson::error_e::CLEAN;

        if ( value.at( "document" ) ) {
          if ( !join.append( "result", *value.at( "document" ), error ) )
            return nullptr;
        }

        std::wstring response = join.json( 0, false );

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementString( "version", L"HTTP/1.1" ) );
        output->append( bson::ElementInt( "code", 200 ) );
        output->append( bson::ElementString( "reason", L"OK" ) );

        bson::Serialize header;
        header.append( bson::ElementInt( "Content-Length", response.size() ) );
        header.append( bson::ElementString( "Content-Type", L"application/json; charset=UTF-8" ) );
        header.append( bson::ElementString( "Connection", "keep-alive" ) );

        if ( !output->append( "header", header, error ) ) return nullptr;
        output->append( bson::ElementString( "body", response ) );

        return output;
      }
    }

    return nullptr;
  }
}
}
