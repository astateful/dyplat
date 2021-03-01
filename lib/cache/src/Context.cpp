#include "astateful/cache/Context.hpp"
#include "astateful/async/net/client/Engine.hpp"

#include "Response/Delete.hpp"
#include "Response/Flush.hpp"
#include "Response/Get.hpp"
#include "Response/Incdec.hpp"
#include "Response/Store.hpp"
#include "Request/Delete.hpp"
#include "Request/Flush.hpp"
#include "Request/Get.hpp"
#include "Request/Incdec.hpp"
#include "Request/Store.hpp"

namespace astateful {
namespace cache {
namespace {
  std::string get( const Context& context, const std::string& key,
                   const std::string& type, uint64_t& cas ) {
    RequestGet request( key, type );
    ResponseGet response( key, cas );

    if ( !context.Send( response, request ) ) return "";

    return response.result();
  }

  result_e store( const Context& context, const std::string& key,
                  const std::string& data, const std::string& type,
                  int flag, int expiry, bool reply, uint64_t cas ) {
    RequestStore request( key, type, flag, expiry, data, cas, reply );

    if ( !reply ) {
      if ( !context.Send( request ) ) return result_e::NO_SERVER;

      return result_e::UNKNOWN;
    }

    ResponseStore response;

    if ( !context.Send( response, request ) ) return result_e::NO_SERVER;

    return response.result();
  }

  std::string inc_dec( const Context& context, const std::string& key,
                       const std::string& type, uint64_t diff, bool reply ) {
    RequestIncdec request( key, type, diff, reply );

    if ( reply ) {
      ResponseIncdec response;

      if ( !context.Send( response, request ) ) return "";

      return response.result();
    }

    context.Send( request );

    return "";
  }
}

  Context::Context( const async::net::client::Engine& engine,
                    const std::wstring& address ) :
    m_engine( engine ),
    m_address( address ) {}

  bool Context::Send( Response& response, const Request& request ) const {
    return m_engine.Send( m_address, request, &response );
  }

  bool Context::Send( const Request& request ) const {
    return m_engine.Send( m_address, request, nullptr );
  }

  bool flush( const Context& context ) {
    ResponseFlush response;
    RequestFlush request;

    if ( !context.Send( response, request ) ) return false;

    return response.result();
  }

  std::string get( const Context& context, const std::string& key ) {
    uint64_t cas;
    return get( context, key, "get", cas );
  }

  std::string gets( const Context& context,
                    const std::string& key,
                    uint64_t& cas ) {
    return get( context, key, "gets", cas );
  }

  result_e del( const Context& context, const std::string& key, bool reply ) {
    RequestDelete request( key, reply );

    if ( !reply ) {
      if ( !context.Send( request ) ) return result_e::NO_SERVER;

      return result_e::UNKNOWN;
    }

    ResponseDelete response;

    if ( !context.Send( response, request ) )
      return result_e::NO_SERVER;

    return response.result();
  }

  result_e set( const Context& context, const std::string& key,
                const std::string& data ) {
    return store( context, key, data, "set", 0, 0, true, -1 );
  }

  result_e add( const Context& context, const std::string& key,
                const std::string& data ) {
    return store( context, key, data, "add", 0, 0, true, -1 );
  }

  result_e replace( const Context& context, const std::string& key,
                    const std::string& data ) {
    return store( context, key, data, "replace", 0, 0, true, -1 );
  }

  result_e append( const Context& context, const std::string& key,
                   const std::string& data ) {
    return store( context, key, data, "append", 0, 0, true, -1 );
  }

  result_e prepend( const Context& context, const std::string& key,
                    const std::string& data ) {
    return store( context, key, data, "prepend", 0, 0, true, -1 );
  }

  result_e cas( const Context& context, const std::string& key,
                const std::string& data, uint64_t cas ) {
    return store( context, key, data, "cas", 0, 0, true, cas );
  }

  std::string inc( const Context& context, const std::string& key,
                   uint64_t diff, bool reply ) {
    return inc_dec( context, key, "incr", diff, reply );
  }

  std::string dec( const Context& context, const std::string& key,
                   uint64_t diff, bool reply ) {
    return inc_dec( context, key, "decr", diff, reply );
  }
}
}
