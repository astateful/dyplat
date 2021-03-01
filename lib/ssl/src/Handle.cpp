#include "astateful/ssl/Handle.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <string>

namespace astateful {
namespace ssl {
  Handle::Handle( ssl_ctx_st * ssl_ctx, int max_pending ) :
    m_max_pending( max_pending ) {
    m_ssl = SSL_new( ssl_ctx );
    m_in = BIO_new( BIO_s_mem() );
    m_out = BIO_new( BIO_s_mem() );

    BIO_set_mem_eof_return( m_in, -1 );
    BIO_set_mem_eof_return( m_out, -1 );

    SSL_set_bio( m_ssl, m_in, m_out );
  }

  Handle::~Handle() {
    SSL_free( m_ssl );
  }

  int Handle::write( const std::string& input ) {
    if ( input.size() > INT_MAX ) return -1;

    return SSL_write( m_ssl,
                      input.data(),
                      static_cast<int>( input.size() ) );
  }

  int Handle::write( const std::vector<uint8_t>& input ) {
    if ( input.size() > INT_MAX ) return -1;

    return SSL_write( m_ssl,
                      input.data(),
                      static_cast<int>( input.size() ) );
  }

  int Handle::read( std::string& output ) {
    return SSL_read( m_ssl,
                     reinterpret_cast<void *>( const_cast<char *>( output.data() ) ),
                     static_cast<int>( output.size() ) );
  }

  int Handle::read( std::vector<uint8_t>& output ) {
    return SSL_read( m_ssl,
                     reinterpret_cast<void *>( output.data() ),
                     static_cast<int>( output.size() ) );
  }

  int Handle::geterror( int return_value ) {
    return SSL_get_error(m_ssl, return_value );
  }

  int Handle::ssl_handshake_finished() {
    return SSL_is_init_finished( m_ssl );
  }

  int Handle::shutdownState() const {
    //SSL_SENT_SHUTDOWN;
    //SSL_RECEIVED_SHUTDOWN;
    return SSL_get_shutdown( m_ssl );
  }

  int Handle::shutdown() {
    return SSL_shutdown( m_ssl );
  }

  bool Handle::update( const std::vector<uint8_t>& input ) {
    int written = 0;

    if ( input.size() > 0 && input.size() <= INT_MAX ) {
      written = BIO_write( m_in,
                           input.data(),
                           static_cast<int>( input.size() ) );
    }
 
    if ( written > 0 ) {
      if ( !SSL_is_init_finished( m_ssl ) ) {
        // Remember to return on any false cases here and let a successful
        // handshake fall through to the callback, otherwise with a branch
        // we would have to wait another iteration before this method would
        // be called again.
        auto ret = SSL_do_handshake( m_ssl );
        if ( ret < 0 ) {
          auto ssl_error = SSL_get_error(m_ssl, ret );
          /*if (ss == 1) {
            auto x = ERR_get_error();
            char buf[120];
            auto y = ERR_error_string(x, buf);
            int halt = 0;
          } else if (ss == 0) {
        return callback( *this );
          }*/
          /*SSL_ERROR_WANT_READ
            SSL_ERROR_WANT_WRITE
            SSL_ERROR_NONE
            SSL_ERROR_ZERO_RETURN
            SSL_ERROR_WANT_CONNECT
            SSL_ERROR_SYSCALL
            SSL_ERROR_SSL*/
          return true;
        } else if ( ret == 0 ) {
          return true;
        }
      }

      //return callback( *this );
    }

    return true;
  }

  int Handle::flush( std::vector<uint8_t>& output ) {
    output.clear();

    size_t num_pending = BIO_ctrl_pending( m_out );
    if ( num_pending > 0 ) {
      size_t constricted = ( num_pending > m_max_pending ) ? m_max_pending : num_pending;
      output.resize( constricted );
      if ( output.size() > INT_MAX ) return -1;

      int read = BIO_read( m_out,
                           output.data(),
                           static_cast<int>( output.size() ) );

      return read;
    }

    return static_cast<int>( num_pending );
  }

  const char * Handle::state() {
    return SSL_state_string( m_ssl );
  }
}
}
