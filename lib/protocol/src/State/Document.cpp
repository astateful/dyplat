#include "State/Document.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Bool.hpp"
#include "astateful/mongo/Iterate.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/plugin/Query.hpp"

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateDocument<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux == "document" ) {
      auto error = bson::error_e::CLEAN;

      // Always check the authentication first since this is our
      // security layer that performs stateless authentication.
      if ( !value.at( "auth" ) ) return nullptr;

      // The connection could actually not even exist at this point if
      // the connection name was never specified so we need to check explicitly.
      if ( value.find( "connection" ) != end( value ) ) {
        if ( value.at( "connection" ) ) {
          std::vector<uint8_t> bson;

          if ( value.at( "connection" )->bson( bson, error ) )
            return std::make_unique<bson::Serialize>( bson );
        }
      }

      // If the authentication value exists then the query
      // parameters have to exist as well!
      auto& query = value.at( "query" );
      if ( query->at( "collection" ) == nullptr ) return nullptr;

      // Create the namespace from the query collection value.
      std::string ns = "astateful.";
      ns += *query->at( "collection" );

      const std::string& method = *( value.at( "method" )->at( "value" ) );

      // In the case of GET, simply request the whole document and return it.
      // This has potential security implications which we will need to fix
      // later but for now its ok.
      if ( method == "GET" ) {
        auto output = std::make_unique<bson::Serialize>();

        // In this case the document id is the same as the data so use this
        // as our query basis and search.
        bson::Serialize document;
        document.append( bson::ElementObjectId( "_id", m_data ) );

        if ( !mongo::find_one( m_context, ns, document, *output, error ) )
          return nullptr;

        return output;
      }

      // For the other HTTP verbs a body is always required since the parameters
      // will be passed within the body, whereas with GET they are passed within
      // the URL.
      if ( !value.at( "body" ) ) return nullptr;

      // Needed to tell the compiler about the conversion that should happen.
      std::string body_value = *value.at( "body" )->at( "value" );

      // Parse the body key value pairs into a query result. We can use the
      // same code that we use for decoding query GET parameters.
      plugin::query_t params;
      plugin::Query query_parse( body_value );
      if ( !query_parse( params ) ) return nullptr;

      if ( method == "POST" ) {
        // Prepare a BSON body which is comprised exactly of the body key value
        // pairs. These pairs will be inserted directly into mongodb.
        bson::Serialize data;
        for ( const auto& param : params ) {
          // Do not append this special id field since it will be chosen by the
          // server as an ObjectId.
          if ( param.first != "_id" )
            data.append( bson::ElementString { param.first, param.second } );
        }

        // Give each newly inserted document a unique ObjectId. Do not rely on the
        // id of the array element itself since this can change at any point.
        data.append( bson::ElementObjectId( "_id" ) );

        // Insert the entire document in one shot.
        if ( !mongo::insert( m_context, ns, data, error ) ) return nullptr;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementBool( "result", true ) );
        return output;
      }
      else if ( method == "PUT" )
      {
        bson::Serialize body;
        for ( const auto& param : params )
        {
          // The id of the document to update is passed within the body?
          if ( param.first != "_id" )
          {
            body.append( bson::ElementString { param.first, param.second } );
          }
        }

        // Prepare the entire document for insertion and be sure to use $set
        // so that we do not overwrite the entire document.
        bson::Serialize data;
        if (!data.append( "$set", body, error )) return nullptr;

        // In this case the document id is the same as the data so use this
        // as our query basis and search.
        bson::Serialize document;
        document.append( bson::ElementObjectId( "_id", params["_id"] ) );

        if ( !mongo::update( m_context,
                             ns,
                             document,
                             data,
                             mongo::update_e::BASIC,
                             error ) ) return nullptr;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementBool( "result", true ) );
        return output;
      }
      else if ( method == "DELETE" )
      {
        // This is the id of the sub document therefore we require that it exists.
        if ( params.find( "_id" ) == end( params ) ) return nullptr;

        // The removal here is quite straightforward, however note that we do not
        // use the id provided by
        bson::Serialize data;
        data.append( bson::ElementObjectId( "_id", params["_id"] ) );

        if ( !mongo::remove( m_context, ns, data, error ) ) return nullptr;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementBool( "result", true ) );
        return output;
      }
    }
    else if ( flux == "id" )
    {
      auto output = std::make_unique<bson::Serialize>();
      output->append( bson::ElementString( "value", m_data ) );
      return output;
    }

    return nullptr;
  }
}
}
