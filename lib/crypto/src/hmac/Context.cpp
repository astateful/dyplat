#include "astateful/crypto/hmac/Context.hpp"

#include "Endian.hpp"

#include <memory>

namespace astateful {
namespace crypto {
namespace hmac {
  void init( Context& context, const void * _K, size_t Klen ) {
    unsigned char pad[64];
    unsigned char khash[32];
    const auto * K = reinterpret_cast<const unsigned char *>( _K );
    size_t i;

    // If Klen > 64, the key is really SHA256(K).
    if ( Klen > 64 ) {
      SHA256_Init( &context.ictx );
      SHA256_Update( &context.ictx, K, Klen );
      SHA256_Final( khash, &context.ictx );

      K = khash;
      Klen = 32;
    }

    // Inner SHA256 operation is SHA256(K xor [block of 0x36] || data).
    SHA256_Init( &context.ictx );

    memset( pad, 0x36, 64 );

    for ( i = 0; i < Klen; i++ ) {
      pad[i] ^= K[i];
    }

    SHA256_Update( &context.ictx, pad, 64 );

    // Outer SHA256 operation is SHA256(K xor [block of 0x5c] || hash).
    SHA256_Init( &context.octx );

    memset( pad, 0x5c, 64 );

    for ( i = 0; i < Klen; i++ ) {
      pad[i] ^= K[i];
    }

    SHA256_Update( &context.octx, pad, 64 );

    memset( khash, 0, 32 ); // Clean the stack.
  }

  void update( Context& context, const void * in, size_t len ) {
    SHA256_Update( &context.ictx, in, len );
  }

  void final( unsigned char digest[32], Context& context ) {
    unsigned char ihash[32];

    SHA256_Final( ihash, &context.ictx ); // Finish the inner SHA256 operation.
    SHA256_Update( &context.octx, ihash, 32 ); // Feed the inner hash to the outer SHA256 operation.
    SHA256_Final( digest, &context.octx ); // Finish the outer SHA256 operation.

    memset( ihash, 0, 32 ); // Clean the stack.
  }
}
}
}
