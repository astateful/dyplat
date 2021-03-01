#include "astateful/ssl/server/Context.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/dh.h>

#include <codecvt>
#include <cassert>
#include <iostream>

namespace astateful {
namespace ssl {
namespace server {
namespace {
  static unsigned char dh512_p[] = {
    0xDA, 0x58, 0x3C, 0x16, 0xD9, 0x85, 0x22, 0x89, 0xD0, 0xE4, 0xAF, 0x75,
    0x6F, 0x4C, 0xCA, 0x92, 0xDD, 0x4B, 0xE5, 0x33, 0xB8, 0x04, 0xFB, 0x0F,
    0xED, 0x94, 0xEF, 0x9C, 0x8A, 0x44, 0x03, 0xED, 0x57, 0x46, 0x50, 0xD3,
    0x69, 0x99, 0xDB, 0x29, 0xD7, 0x76, 0x27, 0x6B, 0xA2, 0xD3, 0xD4, 0x12,
    0xE2, 0x18, 0xF4, 0xDD, 0x1E, 0x08, 0x4C, 0xF6, 0xD8, 0x00, 0x3E, 0x7C,
    0x47, 0x74, 0xE8, 0x33,
  };

  static unsigned char dh512_g[] = {
      0x02,
  };

  static DH *get_dh512() {
    /*DH *dh = NULL;

    if ( ( dh = DH_new() ) == NULL ) return NULL;

    dh->p = BN_bin2bn( dh512_p, sizeof( dh512_p ), NULL );
    dh->g = BN_bin2bn( dh512_g, sizeof( dh512_g ), NULL );

    if ( ( dh->p == NULL ) || ( dh->g == NULL ) ) return NULL;

    return dh;*/
	return nullptr;
  }

  static int alpn_cb( SSL *s,
                      const unsigned char **out,
                      unsigned char *outlen,
                      const unsigned char *in,
                      unsigned int inlen,
                      void *arg ) {
    auto alpn_ctx = static_cast<Context::tlsextalpnctx_st *>( arg );

    if (SSL_select_next_proto
        ((unsigned char **)out, outlen, alpn_ctx->data, alpn_ctx->len, in,
          inlen) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }

    return SSL_TLSEXT_ERR_OK;
  }
}
  Context::tlsextalpnctx_st::tlsextalpnctx_st() :
    data( NULL ),
    len( 0 ) {}

  Context::Context( const std::wstring& cert,
                    const std::wstring& pkey,
                    const std::string& alpn ) :
    ssl::Context( TLSv1_2_server_method() ),
    m_alpn( alpn ) {
    // We would like to support unicode paths however the SSL library
    // can only support traditional single byte strings, so convert.
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    const std::string cert_bytes = convert.to_bytes( cert );
    const std::string pkey_bytes = convert.to_bytes( pkey );

    assert( SSL_CTX_use_certificate_file( *this, cert_bytes.c_str(), 1 ) > 0 );
    assert( SSL_CTX_use_PrivateKey_file( *this, pkey_bytes.c_str(), 1 ) > 0 );

    // Populate the ALPN context.
    unsigned short len;
    m_alpn_ctx.data = next_protos_parse( &len, alpn.data() );
    assert( m_alpn_ctx.data != NULL );
    m_alpn_ctx.len = len;

    // Not sure if we need to do this...
    auto dh = get_dh512();

    SSL_CTX_set_tmp_dh( *this, dh );

    SSL_CTX_set_alpn_select_cb( *this, alpn_cb, &m_alpn_ctx );

    DH_free( dh );
  }

  const std::string& Context::alpn() { return m_alpn; }

  Context::~Context() {
    OPENSSL_free( m_alpn_ctx.data );
  };
}
}
}
