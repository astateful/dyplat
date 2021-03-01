#include "aes/Context.hpp"
#include "Endian.hpp"

#include <memory>

namespace astateful
{
  namespace crypto
  {
    AES::AES( AES_KEY * a, uint64_t b ) :
      key( a ),
      nonce( b ),
      bytectr( 0 )
    {}

    void crypto::AES::operator()( const uint8_t * inbuf, uint8_t * outbuf, size_t buflen )
    {
      uint8_t pblk[16];
      size_t pos;
      int bytemod;

      for ( pos = 0; pos < buflen; pos++ )
      {
        // How far through the buffer are we?
        bytemod = this->bytectr % 16;

        // Generate a block of cipherstream if needed.
        if ( bytemod == 0 )
        {
          be64enc( pblk, this->nonce );
          be64enc( pblk + 8, this->bytectr / 16 );
          AES_encrypt( pblk, this->buf, this->key );
        }

        // Encrypt a byte.
        outbuf[pos] = inbuf[pos] ^ this->buf[bytemod];

        // Move to the next byte of cipherstream.
        this->bytectr += 1;
      }
    }

    AES::~AES()
    {
      // Zero potentially sensitive information.
      for ( int i = 0; i < 16; i++ )
      {
        this->buf[i] = 0;
      }

      this->bytectr = this->nonce = 0;

      memset( this->key, 0, sizeof( AES_KEY ) );
    }
  }
}
