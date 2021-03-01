#include "astateful/ssl/Context.hpp"

#include <openssl/ssl.h>

#include <codecvt>
#include <cassert>

namespace astateful {
namespace ssl {
namespace {
  struct Deleter {
    void operator()( SSL_CTX * ssl_ctx ) {
      SSL_CTX_free( ssl_ctx );
    }
  };
}

  struct Context::pimpl {
    pimpl() = delete;
    pimpl( const ssl_method_st* method ) :
      ssl_ctx( SSL_CTX_new( method ) ) {}

    std::unique_ptr<SSL_CTX, Deleter> ssl_ctx;
  };

  Context::Context( const ssl_method_st* method ) :
                    m_pimpl( std::make_unique<pimpl>( method ) ) {}

  Context::operator ssl_ctx_st*() {
    return m_pimpl->ssl_ctx.get();
  }

  unsigned char * Context::next_protos_parse( unsigned short *outlen,
                                              const char *in ) {
    size_t len;
    unsigned char *out;
    size_t i, start = 0;

    len = strlen(in);
    if (len >= 65535)
        return NULL;

    out = (unsigned char *)OPENSSL_malloc(strlen(in) + 1);
    if (!out) return NULL;

    for (i = 0; i <= len; ++i) {
        if (i == len || in[i] == ',') {
            if (i - start > 255) {
                OPENSSL_free(out);
                return NULL;
            }
            out[start] = i - start;
            start = i + 1;
        } else
            out[i + 1] = in[i];
    }

    *outlen = len + 1;
    return out;
  }

  Context::~Context() {}
}
}