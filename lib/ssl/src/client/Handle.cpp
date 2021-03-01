#include "client/Handle.hpp"

#include <openssl/ssl.h>

namespace astateful {
namespace ssl {
namespace client {
  Handle::Handle( ssl_ctx_st * ssl_ctx,
                  int max_pending ) :
    ssl::Handle( ssl_ctx, max_pending ) {
    SSL_set_connect_state( m_ssl );
  }

  void Handle::handshake() {
    SSL_do_handshake( m_ssl );
  }
}
}
}
