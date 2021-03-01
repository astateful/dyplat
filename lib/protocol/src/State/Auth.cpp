#include "State/Auth.hpp"

#include "astateful/plugin/hmac.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/bson/ObjectId.hpp"
#include "astateful/mongo/Iterate.hpp"

namespace astateful {
namespace protocol {
namespace {
  std::vector<std::string> split( const std::string& s, char delim )
  {
    std::vector<std::string> elems;
    std::stringstream ss( s );
    std::string item;

    while ( std::getline( ss, item, delim ) )
    {
      elems.push_back( item );
    }

    return elems;
  }
}

  template <> std::unique_ptr<bson::Serialize> StateAuth<bson::Serialize, std::string>::operator () (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux != "auth" ) { return nullptr; }

    if ( !value.at( "query" ) ) return nullptr;

    // Find the access token contained in the query string
    // if it exists. This should be improved.
    auto& query = value.at( "query" );
    if ( query->at( "token" ) == nullptr ) return nullptr;

    std::string access_token = *query->at( "token" );

    // Split the access token into its constituent parts.
    auto access_token_part = split( access_token, '|' );
    auto& username = access_token_part[0];
    auto& time = access_token_part[1];

    // Based on the parts parsed from the token, find the user
    // in the database. We need the user to match the password.
    bson::Serialize user_query;
    user_query.append( bson::ElementString( "username", username ) );

    auto error = bson::error_e::CLEAN;

    mongo::Iterate iterate( m_context, "astateful.user", user_query, 1, 0, 0, error );
    if ( !iterate ) return nullptr;

    auto output = std::make_unique<bson::Serialize>();

    // "Steal" the iterate data since it basically represents our user object.
    // Do not use "iterate" any more after this line!
    *output = std::move( *iterate );

    // With the password in hand we can finally match the generated token
    // to the token that was passed along in the query parameters.
    auto hash = plugin::hmac( "website secret", {username, ( *output )["password"], time} );
    if ( hash != access_token_part[2] ) return nullptr;

    return output;
  }
}
}
