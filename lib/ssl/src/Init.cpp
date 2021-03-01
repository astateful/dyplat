#include "astateful/ssl/Init.hpp"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include "Windows.h"

namespace astateful {
namespace ssl {
  struct Init::pimpl {
    pimpl() : num_mutex( CRYPTO_num_locks() ) {
      handle_mutex = static_cast<HANDLE *>( CRYPTO_malloc( num_mutex * sizeof( HANDLE ),
        __FILE__,
        __LINE__ ));

      for ( int i = 0; i < num_mutex; i++ ) {
        handle_mutex[i] = CreateMutex( nullptr, FALSE, nullptr );
      }

      CRYPTO_set_locking_callback( &mutexCallback );
      CRYPTO_set_id_callback( &idCallback );
    }

    static void holdMutex( int mutex ) {
      WaitForSingleObject( handle_mutex[mutex], INFINITE );
    }

    static void releaseMutex( int mutex ) {
      ::ReleaseMutex( handle_mutex[mutex] );
    }

    static void __cdecl mutexCallback( int mode, int mutex, const char * data, int line ) {
      ( mode & CRYPTO_LOCK ) ? holdMutex( mutex ) : releaseMutex( mutex );
    }

    static unsigned long __cdecl idCallback() {
      return static_cast<unsigned long>( GetCurrentThreadId() );
    }

    ~pimpl() {
      for ( int i = 0; i < num_mutex; i++ )
        CloseHandle( handle_mutex[i] );

      CRYPTO_set_locking_callback( nullptr );
      CRYPTO_set_id_callback( nullptr );
      CRYPTO_free( handle_mutex, nullptr, 0 );
    }
  private:
    const int num_mutex;
    static HANDLE * handle_mutex;
  };

  HANDLE * Init::pimpl::handle_mutex = nullptr;

  Init::Init() : m_pimpl( std::make_unique<pimpl>() ) {
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
  }

  Init::~Init() {
    ERR_remove_state(0);
    ENGINE_cleanup();
    CONF_modules_unload(1);
    ERR_free_strings();
    EVP_cleanup();
    sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
    CRYPTO_cleanup_all_ex_data();
  }
}
}