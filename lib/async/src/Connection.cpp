#include "astateful/async/Connection.hpp"

namespace astateful {
namespace async {
  Connection::Connection() : m_buffer( maxBufferSize, 0 ) {
    ZeroMemory( &overlapped, sizeof( OVERLAPPED ) );
  }

  Connection::Connection( const std::vector<uint8_t>& buffer ) :
    m_buffer( buffer ) {
    ZeroMemory( &overlapped, sizeof( OVERLAPPED ) );
  }
}
}