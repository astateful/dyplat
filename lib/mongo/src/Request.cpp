#include "Request.hpp"
#include "Header.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

namespace astateful {
namespace mongo {
  Request::Request( int id, int response, op_e op, int option ) :
    m_op( op ),
    m_header( std::make_unique<Header>( id, response, op ) ) {
    appendInt( option );
  }

  Request::Request( int id, int response, op_e op, int option,
                    const std::string& ns ) :
    m_op( op ),
    m_header( std::make_unique<Header>( id, response, op ) ) {
    appendInt( option );
    m_data.insert( m_data.end(), ns.begin(), ns.end() );
    m_data.push_back( '\0' );
  }

  void Request::appendInt( int32_t input ) {
    auto buffer = reinterpret_cast<char *>( &input );
    for ( int i = 0; i < 4; ++i ) {
      m_data.push_back( *buffer );
      buffer++;
    }
  }

  void Request::appendLong( int64_t input ) {
    auto buffer = reinterpret_cast<char *>( &input );
    for ( int i = 0; i < 8; ++i ) {
      m_data.push_back( *buffer );
      buffer++;
    }
  }

  bool Request::appendSerialize( const Serialize& input, error_e& error ) {
    std::vector<uint8_t> data;

    if ( !input.bson( data, error ) ) return false;

    if ( (error & error_e::FIELD_HAS_DOT) == error_e::FIELD_HAS_DOT )
        error &= ~error_e::FIELD_HAS_DOT;

    m_data.insert( m_data.end(), data.begin(), data.end() );

    return true;
  }

  bool Request::setBody( const Serialize& data,
                         int max_size,
                         error_e& error ) {
    if ( !data.bson( m_body, error ) ) return false;

    if ( m_op == op_e::INSERT ) {
      /*if ( ( error & Error::FIELD_HAS_DOT ) == Error::FIELD_HAS_DOT )
        return false;*/

      if ( ( error & error_e::FIELD_INIT_DOLLAR ) == error_e::FIELD_INIT_DOLLAR )
        return false;
    }

    if ( m_body.size() > max_size ) return false;

    return true;
  }

  bool Request::setBody( const std::vector<Serialize>& data,
                         int max_size,
                         error_e& error ) {
    for ( const auto& serialize : data ) {
      std::vector<uint8_t> bson;

      if ( !serialize.bson( bson, error ) ) return false;

      if ( ( error & error_e::FIELD_HAS_DOT ) == error_e::FIELD_HAS_DOT )
        return false;

      if ( ( error & error_e::FIELD_INIT_DOLLAR ) == error_e::FIELD_INIT_DOLLAR )
        return false;

      m_body.insert( m_body.end(), bson.begin(), bson.end() );
    }

    if ( m_body.size() > max_size ) return false;

    return true;
  }

  void Request::populate() {
    m_buffer = m_header->pack( m_data.size() + m_body.size() + 16 );
    m_buffer.insert( m_buffer.end(), m_data.begin(), m_data.end() );
    m_buffer.insert( m_buffer.end(), m_body.begin(), m_body.end() );
  }

  Request::~Request() {}
}
}
