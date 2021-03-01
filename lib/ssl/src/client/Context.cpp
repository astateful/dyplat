#include "client/Context.hpp"

#include <openssl/ssl.h>

#include <cassert>

namespace astateful {
namespace ssl {
namespace client {
  Context::Context( const std::string& alpn ) :
    ssl::Context( TLSv1_2_client_method() ) {

    unsigned short alpn_len;
    auto parsed_alpn = next_protos_parse( &alpn_len, alpn.data() );
    assert( parsed_alpn != nullptr );

    SSL_CTX_set_alpn_protos( *this, parsed_alpn, alpn_len );
    OPENSSL_free( parsed_alpn );
  }
}
}
}
