#include "astateful/async/pipe/client/Stream.hpp"
#include "astateful/async/pipe/client/Engine.hpp"

namespace astateful {
namespace async {
namespace pipe {
namespace client {
  Stream::Stream( const Engine& engine, const std::string& pipe ) :
    m_buffer(),
    m_engine( engine ),
    m_pipe( pipe ) {}

  void Stream::append( const std::string& buffer ) {
    m_buffer.insert( m_buffer.end(), buffer.begin(), buffer.end() );
  }

  Stream& Stream::operator<<( const std::string& value ) {
    append( value );
    return *this;
  }

  Stream& Stream::operator<<( const char* value ) {
    append( value );
    return *this;
  }

  Stream& Stream::operator<<( int value ) {
    append( std::to_string( value ) );
    return *this;
  }

  Stream& Stream::operator<<( double value ) {
    append( std::to_string( value ) );
    return *this;
  }

  Stream& Stream::operator<<( unsigned long value ) {
    append( std::to_string( value ) );
    return *this;
  }

  Stream& Stream::operator<<( StreamFunctor functor ) {
    return functor( *this );
  }

  int Stream::flush( bool ack ) {
    auto res = m_engine.send( m_pipe, { m_buffer.begin(), m_buffer.end() }, ack );
    m_buffer.clear();
    return res;
  }

  Stream& ack( Stream& context ) {
    context << "\n";
    context.flush( true );
    return context;
  }

  Stream& noack( Stream& context ) {
    context << "\n";
    context.flush( false );
    return context;
  }
}
}
}
}
