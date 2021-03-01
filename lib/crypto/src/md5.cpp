#include "astateful/crypto/md5.hpp"

#include <openssl/md5.h>

#include <cstdint>

namespace astateful {
namespace crypto {
namespace {
  const char hex[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
  };
}

  std::string md5( const std::string& plain ) {
    return md5( std::vector<std::string>{ plain } );
  }

  std::string md5(const std::vector<char>& input, int length) {
    MD5_CTX context;
    uint8_t digest[MD5_DIGEST_LENGTH];
    std::string output( "" );
    output.reserve( 32 );

    MD5_Init( &context );

    int n = 0;
    while ( length > INT_MAX ) {
      MD5_Update( &context, &input[0] + ( n * INT_MAX ), INT_MAX );
      length -= INT_MAX;
      n += 1;
    }

    if ( length > 0 ) {
      MD5_Update( &context, &input[0] + ( n * INT_MAX ), length );
    }

    MD5_Final ( digest, &context );

    for ( int i = 0; i < 16; i++ ) {
      output.push_back( hex[( digest[i] & 0xf0 ) >> 4] );
      output.push_back( hex[digest[i] & 0x0f] );
    }

    return output;
  }

  std::string md5( const std::vector<std::string>& input ) {
    MD5_CTX context;
    uint8_t digest[MD5_DIGEST_LENGTH];
    std::string output;
    output.reserve(33);

    MD5_Init( &context );

    for (const auto entry : input) {
      MD5_Update( &context, entry.c_str(), entry.length() );
    }

    MD5_Final( digest, &context );

    for ( int i = 0; i < 16; i++ ) {
      output.push_back( hex[( digest[i] & 0xf0 ) >> 4] );
      output.push_back( hex[digest[i] & 0x0f] );
    }

    return output;
  }
}
}
