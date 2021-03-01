#include "astateful/mongo/Iterate.hpp"
#include "astateful/mongo/Context.hpp"
#include "Request/Query.hpp"
#include "Request/More.hpp"
#include "Request/Kill.hpp"
#include "Response.hpp"

#include "astateful/async/net/client/Engine.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

#include <cassert>

namespace astateful {
namespace mongo {
  Iterate::Iterate( const Context& context, const std::string& ns,
                    const bson::Serialize& query, const int limit,
                    const int skip, const int option, bson::error_e& error,
                    Serialize* field ) :
    m_current( nullptr ),
    m_response( std::make_unique<Response>() ),
    m_context( context ),
    m_position( 0 ),
    m_limit( limit ),
    m_seen( 0 ),
    m_ns( ns ) {
    RequestQuery request( option, m_ns, skip, m_limit, query, error, field );

    assert( error == bson::error_e::CLEAN );
    assert( m_context.Send( *m_response, request ) );

    while ( !( *m_response ) ) {}

    if ( m_response->count() > 0 ) {
      m_current = std::move( m_response->GenerateSerialize( m_position ) );

      if ( m_response->count() == 1 ) {
        assert( element_e( ( *m_current )["$err"] ) == element_e::EMPTY );
      }

      m_seen += m_response->count();
    }
  }

  bool Iterate::More()
  {
    /*if ( m_limit > 0 && m_seen >= m_limit )
    {
      m_error = MONGO_CURSOR_EXHAUSTED;
      return -1;
    }*/

    auto id = m_response->id();

    if ( !id )
    {
      m_error = Error::EXHAUSTED;
      return false;
    }

    int zero = 0;
    int limit = 0;

    if ( m_limit > 0 ) { limit = m_limit - m_seen; }

    // Implement this check to ensure that the limit has been reached if it
    // it was limited in the first place, or else a query with a 0 limit
    // would be produced, which would return the entire result set.
    if ( limit == 0 && m_limit > 0 ) { return false; }

    m_response = std::make_unique<Response>();

    RequestMore request( zero, m_ns, limit, id );
    if ( !m_context.Send( *m_response, request ) ) return false;

    while ( !( *m_response ) ) {}

    m_position = 0;
    m_seen += m_response->count();

    return true;
  }

  Iterate& Iterate::operator++() {
    if ( m_response->count() == 0 ) {
      if ( m_response->id() ) {
        if ( ( More() != 0 ) || m_response->count() == 0 ) {
          m_error = Error::INVALID;
          return *this;
        }
      }
      else
      {
        m_error = Error::INVALID;
        // Preserve the MONGO_CURSOR_EXHAUSTED
        // error flag set by More ()
        //mpCurrent.reset( nullptr );
        return *this;
      }
    }

    if ( m_position >= m_response->size() )
    {
      if ( !More() )
      {
        m_current.reset( nullptr );
        return *this;
      }

      if ( m_response->count() == 0 )
      {
        /*// Special case for tailable cursors.
        if ( m_bind.id () )
        {
          m_error = Error::PENDING;
          mpCurrent.reset( nullptr );
          return false;
        }

        mpCurrent.reset( nullptr );*/
        return *this;
      }
    }

    m_current = std::move( m_response->GenerateSerialize( m_position ) );

    return *this;
  }

  Iterate::~Iterate() {
    if ( m_response->id() ) {
      RequestKill request( 0, 1, m_response->id() );
      assert( m_context.Send( request ) );
    }
  }
}
}
