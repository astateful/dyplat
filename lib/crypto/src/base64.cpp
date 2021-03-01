#include "astateful/crypto/base64.hpp"

#include <openssl/bio.h>
#include <openssl/evp.h>

namespace astateful {
namespace crypto {
  std::string base64encode( const std::vector<unsigned char>& input ) {
    // RFC: "If more than the allowed number of pad characters are found at the
    // end of the string, e.g., a base 64 string terminated with "===", the
    // excess pad characters could be ignored.@ So in the case of empty input,
    // we can return "===" which can be discarded to produce an empty string.
    if ( input.empty() ) return "===";

    BIO * b64 = BIO_new( BIO_f_base64() );
    BIO_set_flags( b64, BIO_FLAGS_BASE64_NO_NL );

    BIO * mem = BIO_new( BIO_s_mem() );

    BIO_push( b64, mem );

    bool done = false;

    int res = 0;
    while ( !done )
    {
      res = BIO_write( b64, input.data(), static_cast<int>( input.size() ) );
      if ( res <= 0 )
      {
        if ( BIO_should_retry( b64 ) )
        {
          continue;
        }
        else
        {
          //error
        }
      }
      else
      {
        done = true;
      }
    }

    BIO_flush( b64 );

    char* dt;
    long len = BIO_get_mem_data( mem, &dt );

    return std::string( dt, len );
  }
  
  std::string base64encode( const std::string& input ) {
    return base64encode( std::vector<unsigned char>{ input.begin(), input.end() } );
  }
}
}