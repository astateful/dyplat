#include "astateful/bson/ObjectId.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include <time.h>
#include <memory>
#include <cassert>

namespace astateful {
namespace bson {
namespace {
  char to_byte( char c ) {
    if ( c >= '0' && c <= '9' ) return ( c - '0' );
    if ( c >= 'A' && c <= 'F' ) return ( c - 'A' + 10 );
    if ( c >= 'a' && c <= 'f' ) return ( c - 'a' + 10 );
    return 0x0;
  }
}

  int ObjectId::counter = 0;

  ObjectId::ObjectId() {
    m_data.resize( 12 );

    auto timestamp = time( nullptr );
    auto process = GetCurrentProcessId();
    DWORD size = 64;
    char machine[64];

    auto bt = reinterpret_cast<char *>( &timestamp );
    for ( int i = 0; i < 4; ++i ) {
      m_data [4 - i - 1] = *bt;
      *bt++;
    }

    assert( GetComputerNameA( machine, &size ) );
    for ( int i = 4; i < 7; ++i ) {
      m_data[i] = machine[i - 4];
    }

    auto bp = reinterpret_cast<char *>( &process );
    for ( int i = 7; i < 9; ++i ) {
      m_data[i] = *bp;
      *bp++;
    }

    auto bc = reinterpret_cast<char *>( &counter );
    for ( int i = 0; i < 3; ++i ) {
      m_data[12 - i - 1] = *bc;
      *bc++;
    }

    ++counter;
  }

  ObjectId::ObjectId( const std::string& data ) {
    assert( data.size() == 24 );
    m_data.resize( 12 );

    for ( unsigned int i = 0; i != m_data.size(); ++i ) {
      m_data[i] = ( to_byte( data[2 * i] ) << 4 ) | to_byte( data[( 2 * i ) + 1] );
    }
  }

  ObjectId::ObjectId( const std::vector<uint8_t>& data ) : m_data( data ) {}

  ObjectId::operator std::string() const {
    std::string output;

    const char hex[16] = { '0', '1', '2', '3',
                           '4', '5', '6', '7',
                           '8', '9', 'a', 'b',
                           'c', 'd', 'e', 'f' };

    for ( const auto& c: m_data ) {
      output.push_back( hex[( c & 0xf0 ) >> 4] );
      output.push_back( hex[c & 0x0f] );
    }

    return output;
  }
}
}