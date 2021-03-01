#include "astateful/plugin/hmac.hpp"
#include "astateful/crypto/hmac/Context.hpp"

namespace astateful {
namespace plugin {
  std::string hmac( const std::string secret,
                    const std::vector<std::string>& parts ) {
    crypto::hmac::Context context;
    uint8_t digest[32];
    std::string output( "" );
    const char hex[16] = {
      '0', '1', '2', '3',
      '4', '5', '6', '7',
      '8', '9', 'a', 'b',
      'c', 'd', 'e', 'f' };

    crypto::hmac::init( context, secret.c_str(), secret.size() );

    for ( const auto& part : parts ) {
      crypto::hmac::update( context, part.data(), part.size() );
    }

    crypto::hmac::final( digest, context );

    for ( int i = 0; i < 32; i++ ) {
      output.push_back( hex[( digest[i] & 0xf0 ) >> 4] );
      output.push_back( hex[digest[i] & 0x0f] );
    }

    return output;
  }
}
}