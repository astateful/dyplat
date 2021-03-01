#include "State/Token.hpp"

#include "astateful/plugin/hmac.hpp"
#include "astateful/plugin/Query.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/mongo/Context.hpp"

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateToken<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& ) const {
    const std::string& method = *( value.at( "method" )->at( "value" ) );

    bson::Serialize response;
    auto error = bson::error_e::CLEAN;

    if ( m_data == "obtain" && method == "POST" ) {
      // Needed to tell the compiler about the conversion that should happen.
      std::string body_value = *value.at( "body" )->at( "value" );

      // Parse the body key value pairs into a query result. We can use the
      // same code that we use for decoding query GET parameters.
      plugin::query_t params;
      plugin::Query query_parse( body_value );
      if ( !query_parse( params ) ) return nullptr;

      bson::Serialize user;
      user.append( bson::ElementString( "username", params["username"] ) );
      user.append( bson::ElementString( "password", params["password"] ) );

      // Declare an empty field since we do not care about the fields.
      bson::Serialize result;

      // Make sure that this user already exists in the database.
      if ( !mongo::find_one( m_context, "astateful.user", user, result, error ) )
        return nullptr;

      // Generate a token based on these input parameters.
      int expires = 3600;
      auto hash = plugin::hmac( "website secret", {
        params["username"],
        params["password"],
        std::to_string( expires ) } );

      response.append( bson::ElementString( "access_token", params["username"] + "|" + std::to_string( expires ) + "|" + hash ) );
      response.append( bson::ElementString( "token_type", "example" ) );
      response.append( bson::ElementInt( "expires_in", expires ) );
      response.append( bson::ElementString( "refresh_token", "tGzv3JOkF0XG5Qx2TlKWIA" ) );
    }

    auto output = std::make_unique<bson::Serialize>();
    output->append( bson::ElementString( "version", L"HTTP/1.1" ) );
    output->append( bson::ElementInt( "code", 200 ) );
    output->append( bson::ElementString( "reason", L"OK" ) );

    std::wstring body = response.json( 0, false );

    bson::Serialize header;
    header.append( bson::ElementInt( "Content-Length", body.size() ) );
    header.append( bson::ElementString( "Content-Type", L"application/json;charset=UTF-8" ) );
    header.append( bson::ElementString( "Cache-Control", L"no-store" ) );
    header.append( bson::ElementString( "Pragma", L"no-cache" ) );
    header.append( bson::ElementString( "Connection", "keep-alive" ) );

    if ( !output->append( "header", header, error ) ) return nullptr;

    output->append( bson::ElementString( "body", body ) );

    return output;
  }
}
}
