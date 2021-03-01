#include "astateful/ssl/server/Handle.hpp"

#include <openssl/ssl.h>

#define SSL_WHERE_INFO(ssl, w, flag, msg) {                \
    if(w & flag) {                                         \
      printf("%20.20s", msg);                              \
      printf(" - %30.30s ", SSL_state_string_long(ssl));   \
      printf(" - %5.10s ", SSL_state_string(ssl));         \
      printf("\n");                                        \
    }                                                      \
  } 

namespace astateful {
namespace ssl {
namespace server {
namespace {
  void ssl_info_callback( const SSL* ssl, int where, int ret ) {
    if ( ret == 0 ) {
      printf("-- krx_ssl_info_callback: error occured.\n");
      return;
    }
 
    SSL_WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
    SSL_WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_START, "HANDSHAKE START");
    SSL_WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
  }
}

  Handle::Handle( ssl_ctx_st * ssl_ctx,
                  int max_pending ) :
    ssl::Handle( ssl_ctx, max_pending ) {
    SSL_set_accept_state( m_ssl );
    SSL_set_info_callback( m_ssl, ssl_info_callback );
  }
}
}
}
