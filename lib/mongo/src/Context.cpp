#include "Response.hpp"
#include "Request/Remove.hpp"
#include "Request/Insert.hpp"
#include "Request/Update.hpp"

#include "astateful/mongo/Iterate.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Bool.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/crypto/md5.hpp"
#include "astateful/async/net/client/Engine.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
namespace {
  std::string db_from_ns( const std::string& ns ) {
    auto dot_pos = ns.find( "." );
    if ( dot_pos != std::string::npos ) return ns.substr( 0, dot_pos );
    return "";
  }

  std::string collection_from_ns( const std::string& ns ) {
    auto dot_pos = ns.find( "." );
    if ( dot_pos != std::string::npos ) return ns.substr( dot_pos + 1 );
    return "";
  }

  Serialize generate_write_concern( int w, int wtimeout, int j, int fsync,
                                    const std::string mode ) {
    Serialize concern;

    concern.append( ElementInt( "getlasterror", 1 ) );

    if ( mode.size() > 0 ) {
      concern.append( ElementString( "w", mode ) );
    } else if ( w > 1 ) {
      concern.append( ElementInt( "w", w ) );
    }

    if ( wtimeout ) concern.append( ElementInt( "wtimeout", wtimeout ) );
    if ( j ) concern.append( ElementInt( "j", j ) );
    if ( fsync ) concern.append( ElementInt( "fsync", fsync ) );

    return concern;
  }

  bool run( const Context& context, const std::string& db,
            const Serialize& query, Serialize& response, error_e& error ) {
    auto command = db + ".$cmd";

    if ( !find_one( context, command, query, response, error ) ) return false;

    double ok = response["ok"];
    if ( ok != 1 ) return false;

    return true;
  }

  bool run_error( const Context& context, const std::string& db,
                  Serialize& output, const std::string& key, error_e& error ) {
    Serialize query;
    query.append( ElementInt( key, 1 ) );

    if ( run( context, db, query, output, error ) ) {
      // If this field exists, and is not empty, it means that we
      // have an error and should return a failure status.
      if ( element_e( output["err"] ) != element_e::EMPTY )
        return false;

      return true;
    }

    return false;
  }

  bool run_write_concern( const Context& context, const std::string& ns,
                          error_e& error ) {
    Serialize result;

    // Convert the inputted namespace to a command which is run on the
    // database itself. The database is contained within the entire
    // namespace string so it must be parsed out.
    auto cmd = db_from_ns( ns ) + ".$cmd";

    if ( find_one( context, cmd, context.writeConcern, result, error ) ) {
      // If this field exists, and is not of type null, it means
      // that there is an error present.
      if ( element_e( result["$err"] ) == element_e::STRING )
        return false;

      if ( element_e( result["err"] ) == element_e::STRING )
        return false;

      return true;
    }

    return false;
  }
}

  Context::Context( const async::net::client::Engine& engine,
                    const std::wstring& address, error_e& error ) :
    maxBsonSize( -1 ),
    writeConcern( std::move( generate_write_concern( 1, 50, 1, 0, "" ) ) ),
    isMaster( false ),
    m_engine( engine ),
    m_address( address ) {
    Serialize response;
    Serialize query;
    query.append( ElementInt( "ismaster", 1 ) );

    assert( run( *this, "admin", query, response, error ) );

    isMaster = response["ismaster"];
    maxBsonSize = response["maxBsonObjectSize"];
  }

  bool Context::Send( Response& response, const Request& request ) const {
    return m_engine.Send( m_address, request, &response );
  }

  bool Context::Send( const Request& request ) const {
    return m_engine.Send( m_address, request );
  }

  bool find_one( const Context& context, const std::string& ns,
                 const Serialize& query, Serialize& output,
                 error_e& error, Serialize* field ) {
    Iterate iterate( context, ns, query, 1, 0, 0, error, field );
    if ( iterate ) {
      output = std::move( *iterate );
      return true;
    }

    return false;
  }

  bool remove( const Context& context, const std::string& ns,
               const Serialize& query, error_e& error ) {
    RequestRemove request( 0, ns, 0, context.maxBsonSize, query, error );

    if ( !context.Send( request ) ) return false;

    return run_write_concern( context, ns, error );
  }

  bool insert( const Context& context, const std::string& ns,
               const Serialize& data, error_e& error ) {
    RequestInsert request( 0, ns, context.maxBsonSize, data, error );

    if ( !context.Send( request ) ) return false;

    return run_write_concern( context, ns, error );
  }

  bool ping( const Context& context, Serialize& result, error_e& error ) {
    Serialize query;
    query.append( ElementInt( "ping", 1 ) );

    return run( context, "admin", query, result, error );
  }

  bool drop_collection( const Context& context, const std::string& db,
                        const std::string& collection, Serialize& result,
                        error_e& error ) {
    Serialize query;
    query.append( ElementString( "drop", collection ) );

    return run( context, db, query, result, error );
  }

  bool drop_database( const Context& context, const std::string& db,
                      Serialize& result, error_e& error ) {
    Serialize query;
    query.append( ElementInt( "dropDatabase", 1 ) );

    return run( context, db, query, result, error );
  }

  bool reset_error( const Context& context, const std::string& db,
                    Serialize& result, error_e& error ) {
    Serialize query;
    query.append( ElementInt( "reseterror", 1 ) );

    return run( context, db, query, result, error );
  }

  bool get_previous_error( const Context& context, const std::string& db,
                           Serialize& result, error_e& error ) {
    return run_error( context, db, result, "getpreverror", error );
  }

  bool get_last_error( const Context& context, const std::string& db,
                       Serialize& result, error_e& error ) {
    return run_error( context, db, result, "getlasterror", error );
  }

  bool update( const Context& context, const std::string& ns,
               const Serialize& condition, const Serialize& data,
               update_e update, error_e& error ) {
    RequestUpdate request( 0, ns, update, context.maxBsonSize, condition,
                           data, error );

    if ( !context.Send( request ) ) return false;

    return run_write_concern( context, ns, error );
  }

  bool add_user( const Context& context, const std::string& db,
                 const std::string& username, const std::string& password,
                 error_e& error ) {
    auto ns = db + ".system.users";
    auto hash = crypto::md5( { username, ":mongo:", password } );

    Serialize user;
    user.append( ElementString( "user", username ) );

    Serialize sub;
    sub.append( ElementString( "pwd", hash ) );

    Serialize pass;
    if ( !pass.append( "$set", sub, error ) ) return false;

    return update( context, ns, user, pass, update_e::UPSERT, error );
  }

  bool create_index( const Context& context, const std::string& ns,
                     const Serialize& key, const std::string& name,
                     int options, int ttl, Serialize& cResult, error_e& error ) {
    Serialize query;

    if ( !query.append( "key", key, error ) ) return false;
    query.append( ElementString( "ns", ns ) );
    query.append( ElementString( "name", name ) );

    if ( options & static_cast<int>( index_e::UNIQUE ) )
      query.append( ElementBool( "unique", true ) );

    if ( options & static_cast<int>( index_e::DROP_DUPS ) )
      query.append( ElementBool( "dropDups", 1 ) );

    if ( options & static_cast<int>( index_e::BACKGROUND ) )
      query.append( ElementBool( "background", 1 ) );

    if ( options & static_cast<int>( index_e::SPARSE ) )
      query.append( ElementBool( "sparse", 1 ) );

    if ( ttl > 0 ) query.append( ElementInt( "expireAfterSeconds", ttl ) );

    auto db = db_from_ns( ns );
    auto db_index_ns = db + ".system.indexes";

    if ( !insert( context, db_index_ns, query, error ) ) return false;

    return get_last_error( context, db, cResult, error );
  }

  bool create_simple_index( const Context& context, const std::string& ns,
                            const std::string& field, const std::string& name,
                            int option, Serialize& output, error_e& error ) {
    Serialize key;
    key.append( ElementInt( field, 1 ) );

    return create_index( context, ns, key, name, option, -1, output, error );
  }

  bool create_capped_col( const Context& context, const std::string& ns, int size,
                          int max, Serialize& result, error_e& error ) {
    Serialize query;
    query.append( ElementString( "create", collection_from_ns( ns ) ) );
    query.append( ElementBool( "capped", true ) );
    query.append( ElementInt( "size", size ) );

    if ( max > 0 ) query.append( ElementInt( "max", max ) );

    return run( context, db_from_ns( ns ), query, result, error );
  }

  bool get_master_status( const Context& context, Serialize& result,
                          error_e& error ) {
    Serialize query;
    query.append( ElementInt( "ismaster", 1 ) );

    if ( !run( context, "admin", query, result, error ) ) return false;

    return result["ismaster"];
  }

  bool insert_batch( const Context& context, const std::string& ns,
                     const std::vector<Serialize>& data,
                     bool continue_on_failure, error_e& error ) {
    RequestInsert request( continue_on_failure ? 1 : 0, ns,
                           context.maxBsonSize, data, error );

    if ( !context.Send( request ) ) return false;

    return run_write_concern( context, ns, error );
  }

  bool count( const Context& context, const std::string& ns, error_e& error,
              Serialize& result, const Serialize * query ) {
    Serialize command;
    command.append( ElementString( "count", collection_from_ns( ns ) ) );

    if ( query ) {
      if ( !command.append( "query", *query, error ) ) return false;
    }

    return run( context, db_from_ns( ns ), command, result, error );
  }

  bool md5_lookup( const Context& context, const std::string& db,
                   const ObjectId& oid, const std::string& root,
                   Serialize& result, error_e& error ) {
    Serialize query;
    query.append( ElementObjectId( "filemd5", oid ) );
    query.append( ElementString( "root", root ) );

    return run( context,db, query, result, error );
  }
}
}
