#include "State/Connection.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Bool.hpp"
#include "astateful/bson/Iterate.hpp"
#include "astateful/bson/ObjectId.hpp"
#include "astateful/mongo/Iterate.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/plugin/Query.hpp"

#include <iostream>

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateConnection<bson::Serialize, std::string>::operator () (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux == "connection" )
    {
      // Always check the authentication first since this is our
      // security layer that performs stateless authentication.
      if ( !value.at( "auth" ) ) return nullptr;

      // This is the id of the parent document. When within a connection
      // we always need the parent for a connection to make sense.
      if ( !value.at( "id" ) ) return nullptr;

      // If the authentication value exists then the query
      // parameters have to exist as well so assume they exist.
      auto& query = value.at( "query" );
      if ( query->at( "collection" ) == nullptr ) return nullptr;

      // Create the namespace from the query collection value.
      std::string ns = "astateful.";
      ns += *query->at( "collection" );

      // Declare the base document query using the id obtained from the
      // parent document. As stated before, we always need the root document.
      bson::Serialize document;
      std::string id = *value.at( "id" )->at( "value" );
      document.append( bson::ElementObjectId( "_id", id ) );

      const std::string& method = *( value.at( "method" )->at( "value" ) );

      auto error = bson::error_e::CLEAN;

      // In this case, there are two potential cases: either there is an additional
      // id parameter or there is not. If there is not, return all documents pertaining
      // to this connection. If there is, filter the documents according to the given
      // id and return that. The id specified here is NOT the id of the document.
      if ( method == "GET" )
      {
        if ( query->at( "id" ) == nullptr )
        {
          bson::Serialize result;
          if ( !mongo::find_one( m_context, ns, document, result, error ) )
            return nullptr;

          return std::make_unique<bson::Serialize>( result.at( m_data )->bson( error ) );
        }
        else
        {
          // Needed to tell the compiler about the conversion that should happen.
          std::string id = *query->at( "id" );

          document.append( bson::ElementObjectId( m_data + "._id", id ) );

          bson::Serialize result;
          if ( !mongo::find_one( m_context, ns, document, result, error ) )
            return nullptr;

          std::vector<uint8_t> data;
          if ( !result.bson( data, error ) ) return nullptr;

          bson::Iterate iterate( data );

          while ( iterate )
          {
            if ( iterate->key() == "posts" )
            {
              std::vector<uint8_t> k( *iterate );

              bson::Iterate asd( k );

              while ( asd )
              {
                std::vector<uint8_t> ko( *asd );

                bson::Iterate g( ko );

                while ( g )
                {
                  /*if ( *g == id )
                  {
                    return std::make_unique<bson::Serialize>( ko );
                  }*/

                  ++g;
                }

                ++asd;
              }
            }

            ++iterate;
          }
        }
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

      if ( method == "POST" )
      {
        // Prepare a BSON body which is comprised exactly of the body key value
        // pairs. These pairs will be inserted directly into mongodb.
        bson::Serialize body;
        for ( const auto& param : params )
        {
          // Do not append this special id field since it will be chosen by the
          // server as an ObjectId.
          if ( param.first != "_id" )
          {
            body.append( bson::ElementString { param.first, param.second } );
          }
        }

        // Give each newly inserted document a unique ObjectId. Do not rely on the
        // id of the array element itself since this can change at any point.
        body.append( bson::ElementObjectId( "_id" ) );

        // Prepare the entire document to be inserted. It is important to use the
        // $push modifier since this will create an entirely new document. In order
        // that we do not overwrite the entire document, we prepare it differently
        // so that we only push to the connection.

        bson::Serialize sub;
        if ( !sub.append( m_data, body, error ) ) return nullptr;

        bson::Serialize data;
        if ( !data.append( "$push", sub, error ) ) return nullptr;

        if ( !mongo::update( m_context,
          ns,
          document,
          data,
          mongo::update_e::BASIC,
          error ) ) return nullptr;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementBool( "result", true ) );
        return output;
      } else if ( method == "PUT" ) {
        // This is the id of the sub document therefore we require that it exists.
        if ( params.find( "_id" ) == end( params ) ) return nullptr;

        // Prepare the update query. We need to put the id of the sub document
        // in the base query along with the original document so that mongo knows
        // which sub document should be updated.
        document.append( bson::ElementObjectId( m_data + "._id", params["_id"] ) );

        bson::Serialize body;
        for ( const auto& param : params ) {
          // No need to include the id in the condition, since its already
          // a part of the base document query.
          if ( param.first != "_id" )
            body.append( bson::ElementString { m_data + ".$." + param.first, param.second } );
        }

        bson::Serialize data;
        data.append( "$set", body, error );

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

        // Prepare the removal query, using the $pull operator to pull from the
        // array contained within the document. This means we do an update of the
        // document in order to remove the sub document.
        bson::Serialize body;
        body.append( bson::ElementObjectId( "_id", params["_id"] ) );

        bson::Serialize sub;
        sub.append( m_data, body, error );

        bson::Serialize data;
        data.append( "$pull", sub, error );

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
    }

    return nullptr;
  }
}
}
