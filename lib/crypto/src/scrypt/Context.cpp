#include "astateful/crypto/scrypt/Context.hpp"
#include "astateful/crypto/hmac/Context.hpp"

#include "smix/Context.hpp"
#include "aes/Context.hpp"
#include "Endian.hpp"

#include <Windows.h>

#include <assert.h>
#include <time.h>
#include <errno.h>

namespace astateful {
namespace crypto {
namespace scrypt {
namespace {
  void PBKDF2_SHA256( const std::vector <uint8_t>& vPassword,
                      const uint8_t * salt,
                      size_t saltlen,
                      uint64_t c,
                      uint8_t * buf,
                      size_t dkLen )
  {
    hmac::Context PShctx, hctx;
    size_t i;
    uint8_t ivec[4];
    uint8_t U[32];
    uint8_t T[32];
    uint64_t j;
    int k;
    size_t clen;

    // Compute HMAC state after processing P and S.
    hmac::init( PShctx, &vPassword[0], vPassword.size() );
    hmac::update( PShctx, salt, saltlen );

    // Iterate through the blocks.
    for ( i = 0; i * 32 < dkLen; i++ )
    {
      // Generate INT(i + 1).
      be32enc( ivec, static_cast <uint32_t> ( i + 1 ) );

      // Compute U_1 = PRF(P, S || INT(i)).
      memcpy( &hctx, &PShctx, sizeof( hmac::Context ) );

      hmac::update( hctx, ivec, 4 );
      hmac::final( U, hctx );

      // T_i = U_1 ...
      memcpy( T, U, 32 );

      for ( j = 2; j <= c; j++ )
      {
        // Compute U_j.
        hmac::init( hctx, &vPassword[0], vPassword.size() );
        hmac::update( hctx, U, 32 );
        hmac::final( U, hctx );

        // ... xor U_j ...
        for ( k = 0; k < 32; k++ )
        {
          T[k] ^= U[k];
        }
      }

      // Copy as many bytes as necessary into buf.
      clen = dkLen - i * 32;
      if ( clen > 32 ) { clen = 32; }

      memcpy( &buf[i * 32], T, clen );
    }

    // Clean PShctx, since we never called _Final on it.
    memset( &PShctx, 0, sizeof( hmac::Context ) );
  }

  //! Compute scrypt(passwd[0 .. passwdlen - 1], salt[0 .. saltlen -
  //! 1], N, r, p, buflen) and write the result into buf.  The
  //! parameters r, p, and buflen must satisfy r * p < 2^30 and
  //! buflen <= (2^32 - 1) * 32.  The parameter N must be a power of
  //! 2 greater than 1.
  //!
  int scrypt( const std::vector <uint8_t>& vPassword,
              const uint8_t * salt,
              size_t saltlen,
              uint64_t N,
              uint32_t r,
              uint32_t p,
              uint8_t * buf,
              size_t buflen )
  {
    SMIX mix;
    void * B0, *V0, *XY0;
    uint8_t * B;
    uint32_t * V;
    uint32_t * XY;
    uint32_t i;

    // Sanity-check parameters.
    if ( static_cast <uint64_t> ( r )* static_cast <uint64_t> ( p ) >= ( 1 << 30 ) )
    {
      errno = EFBIG;
      return -1;
    }

    if ( ( ( N & ( N - 1 ) ) != 0 ) || ( N == 0 ) )
    {
      errno = EINVAL;
      return -1;
    }

    if ( ( r > SIZE_MAX / 128 / p ) ||
#if SIZE_MAX / 256 <= UINT32_MAX
          ( r > SIZE_MAX / 256 ) ||
#endif
          ( N > SIZE_MAX / 128 / r ) )
    {
      errno = ENOMEM;
      return -1;
    }

    // Allocate memory.

    if ( ( B0 = malloc( 128 * r * p + 63 ) ) == nullptr )
    {
      return -1;
    }

    B = ( uint8_t * )( ( ( uintptr_t )( B0 ) + 63 ) & ~( uintptr_t )( 63 ) );

    if ( ( XY0 = malloc( 256 * r + 64 + 63 ) ) == nullptr )
    {
      free( B0 );
    }

    XY = ( uint32_t * )( ( ( uintptr_t )( XY0 ) + 63 ) & ~( uintptr_t )( 63 ) );

    if ( ( V0 = malloc( 128 * r * N + 63 ) ) == nullptr )
    {
      free( XY0 );
    }

    V = ( uint32_t * )( ( ( uintptr_t )( V0 ) + 63 ) & ~( uintptr_t )( 63 ) );

    // 1: (B_0 ... B_{p-1}) <-- PBKDF2(P, S, 1, p * MFLen)
    PBKDF2_SHA256(
      vPassword,
      salt,
      saltlen,
      1,
      B,
      p * 128 * r
    );

    // 2: for i = 0 to p - 1 do
    for ( i = 0; i < p; i++ )
    {
      // 3: B_i <-- MF(B_i, N)
      mix( &B[i * 128 * r], r, N, V, XY );
    }

    // 5: DK <-- PBKDF2(P, B, 1, dkLen)
    PBKDF2_SHA256(
      vPassword,
      B,
      p * 128 * r,
      1,
      buf,
      buflen
    );

    free( V0 );
    free( XY0 );
    free( B0 );

    return 0;
  }

  void get_clock_resolution( double& dOutput )
  {
    dOutput = 1.0 / CLOCKS_PER_SEC;
  }

  void get_clock_difference( LARGE_INTEGER * start, double * dOutput, LARGE_INTEGER * frequency )
  {
    LARGE_INTEGER finish;

    QueryPerformanceCounter( &finish );

    *dOutput = ( ( finish.QuadPart - start->QuadPart ) / ( ( ( double )frequency->QuadPart / CLOCKS_PER_SEC ) ) );
  }

  //! Estimate the number of salsa20/8 cores which can be executed
  //! per second, and return the value via opps.
  //!
  result_e estimate_cpu_perf( double& dOutput )
  {
    std::vector <uint8_t> empty( 1 );
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    double resd, diffd;
    uint64_t i = 0;

    get_clock_resolution( resd );

    QueryPerformanceFrequency( &frequency );
    QueryPerformanceCounter( &start );

    do
    {
      // Do an scrypt.
      if ( scrypt( empty, nullptr, 0, 16, 1, 1, nullptr, 0 ) )
      {
        return result_e::KEY_FAIL;
      }

      // Has the clock ticked?
      get_clock_difference( &start, &diffd, &frequency );

      if ( diffd > 0 ) { break; }
    }
    while ( true );

    // Could how many scrypts we can do before the next tick.
    QueryPerformanceCounter( &start );

    do
    {
      // Do an scrypt.
      if ( scrypt( empty, nullptr, 0, 128, 1, 1, nullptr, 0 ) )
      {
        return result_e::KEY_FAIL;
      }

      // We invoked the salsa20/8 core 512 times.
      i += 512;

      // Check if we have looped for long enough.
      get_clock_difference( &start, &diffd, &frequency );

      if ( diffd > resd ) { break; }
    }
    while ( true );

    // We can do approximately i salsa20/8 cores per diffd seconds.
    dOutput = i / diffd;

    return result_e::SUCCESS;
  }

  //! Examine the system and return via memlimit the amount of RAM
  //! which should be used -- the specified fraction of the available
  //! RAM, but no more than maxmem, and no less than 1MiB.
  //!
  int usable_memory( const Context& context, size_t& iOutput )
  {
    size_t sysconf_memlimit;
    size_t memavail;

    // Get memory limits.
    sysconf_memlimit = static_cast <size_t> ( -1 );

    MEMORYSTATUSEX status;
    status.dwLength = sizeof( status );
    GlobalMemoryStatusEx( &status );
    sysconf_memlimit = status.ullAvailPhys;

    memavail = static_cast <size_t> ( context.fraction ) * sysconf_memlimit;

    // Don't use more than the specified maximum.
    if ( ( context.memory > 0 ) && ( memavail > context.memory ) )
    {
      memavail = context.memory;
    }

    // But always allow at least 1 MiB.
    if ( memavail < 1048576 )
    {
      memavail = 1048576;
    }

    // Return limit via the provided pointer.
    iOutput = memavail;

    return 0;
  }

  result_e create_salt( std::vector<uint8_t>& vOutput )
  {
    for ( int i = 0; i < 32; i++ )
    {
      vOutput.push_back( 'a' );
    }

    return result_e::SUCCESS;
  }

  result_e check_param( const Context& context, int logN, uint32_t r, uint32_t p )
  {
    size_t memlimit;
    double opps;
    double opslimit;
    uint64_t N;
    result_e result;

    // Figure out the maximum amount of memory we can use.
    if ( usable_memory( context, memlimit ) )
    {
      return result_e::MEM_FAIL;
    }

    // Figure out how fast the CPU is.
    if ( ( result = estimate_cpu_perf( opps ) ) != result_e::SUCCESS )
    {
      return result;
    }

    opslimit = opps * context.time;

    // Sanity-check values.
    if ( ( logN < 1 ) || ( logN > 63 ) )
    {
      return result_e::DATA_INPUT_FAIL;
    }

    if ( static_cast <uint64_t> ( r )* static_cast <uint64_t> ( p ) >= 0x40000000 )
    {
      return result_e::DATA_INPUT_FAIL;
    }

    // Check limits.
    N = static_cast <uint64_t> ( 1 ) << logN;

    if ( ( memlimit / N ) / r < 128 )
    {
      return result_e::DECRYPT_MEM_FAIL;
    }

    if ( ( opslimit / N ) / ( r * p ) < 4 )
    {
      return result_e::DECRYPT_TIME_FAIL;
    }

    return result_e::SUCCESS;
  }

  result_e pick_param( const Context& context, int& logN, uint32_t& r, uint32_t& p )
  {
    size_t memlimit;
    double opps;
    double opslimit;
    double maxN, maxrp;
    result_e result;

    // Figure out how much memory to use.
    if ( usable_memory( context, memlimit ) ) { return result_e::MEM_FAIL; }

    // Figure out how fast the CPU is.
    if ( ( result = estimate_cpu_perf( opps ) ) != result_e::SUCCESS )
    {
      return result;
    }

    opslimit = opps * context.time;

    // Allow a minimum of 2^15 salsa20/8 cores.
    if ( opslimit < 32768 ) { opslimit = 32768; }

    // Fix r = 8 for now.
    r = 8;

    // The memory limit requires that 128Nr <= memlimit, while the CPU
    // limit requires that 4Nrp <= opslimit.  If opslimit < memlimit/32,
    // opslimit imposes the stronger limit on N.
    if ( opslimit < memlimit / 32 )
    {
      // Set p = 1 and choose N based on the CPU limit.
      p = 1;
      maxN = opslimit / ( r * 4 );
      for ( logN = 1; logN < 63; logN += 1 )
      {
        if ( static_cast <uint64_t> ( 1 ) << logN > maxN / 2 )
        {
          break;
        }
      }
    }
    else
    {
      // Set N based on the memory limit.
      maxN = static_cast <double> ( memlimit ) / ( r * 128 );
      for ( logN = 1; logN < 63; logN += 1 )
      {
        if ( static_cast <uint64_t> ( 1 ) << logN > maxN / 2 )
        {
          break;
        }
      }

      // Choose p based on the CPU limit.
      maxrp = ( opslimit / 4 ) / ( static_cast <uint64_t> ( 1 ) << logN );
      if ( maxrp > 0x3fffffff )
      {
        maxrp = 0x3fffffff;
      }

      p = static_cast <uint32_t> ( maxrp ) / r;
    }

    return result_e::SUCCESS;
  }

  result_e decrypt_params( const Context& context,
                           const uint8_t header[96],
                           uint8_t dk[64] )
  {
    uint8_t salt[32];
    uint8_t hbuf[32];
    int logN;
    uint32_t r;
    uint32_t p;
    uint64_t N;
    SHA256_CTX ctx;
    uint8_t * key_hmac = &dk[32];
    hmac::Context hctx;
    result_e rc;

    // Parse N, r, p, salt.
    logN = header[7];
    r = be32dec( &header[8] );
    p = be32dec( &header[12] );
    memcpy( salt, &header[16], 32 );

    // Verify header checksum.
    SHA256_Init( &ctx );
    SHA256_Update( &ctx, header, 48 );
    SHA256_Final( hbuf, &ctx );

    if ( memcmp( &header[48], hbuf, 16 ) )
    {
      return result_e::DATA_INPUT_FAIL;
    }

    // Check whether the provided parameters are valid and whether the
    // key derivation function can be computed within the allowed memory
    // and CPU time.
    if ( ( rc = check_param( context, logN, r, p ) ) != result_e::SUCCESS )
    {
      return ( rc );
    }

    // Compute the derived keys.
    N = static_cast <uint64_t> ( 1 ) << logN;

    if ( scrypt( context.password, salt, 32, N, r, p, dk, 64 ) )
    {
      return result_e::KEY_FAIL;
    }

    // Check header signature (i.e., verify password).
    hmac::init( hctx, key_hmac, 32 );
    hmac::update( hctx, header, 64 );
    hmac::final( hbuf, hctx );

    if ( memcmp( hbuf, &header[64], 32 ) )
    {
      return result_e::PASSWORD_FAIL;
    }

    return result_e::SUCCESS;
  }

  result_e encrypt_params( const Context& context,
                           uint8_t header[96],
                           uint8_t dk[64] )
  {
    std::vector <uint8_t> salt;
    uint8_t hbuf[32];
    int logN;
    uint64_t N;
    uint32_t r;
    uint32_t p;
    SHA256_CTX ctx;
    uint8_t * key_hmac = &dk[32];
    hmac::Context hctx;
    result_e result;

    // Pick values for N, r, p.
    if ( ( result = pick_param( context, logN, r, p ) ) != result_e::SUCCESS )
    {
      return result;
    }

    N = static_cast <uint64_t> ( 1 ) << logN;

    // Get some salt.
    if ( ( result = create_salt( salt ) ) != result_e::SUCCESS )
    {
      return result;
    }

    // Generate the derived keys.
    if ( scrypt( context.password, &salt[0], salt.size(), N, r, p, dk, 64 ) )
    {
      return result_e::KEY_FAIL;
    }

    // Construct the file header.
    memcpy( header, "scrypt", 6 );

    header[6] = 0;
    header[7] = logN;

    be32enc( &header[8], r );
    be32enc( &header[12], p );

    memcpy( &header[16], &salt[0], salt.size() );

    // Add header checksum.
    SHA256_Init( &ctx );
    SHA256_Update( &ctx, header, 48 );
    SHA256_Final( hbuf, &ctx );

    memcpy( &header[48], hbuf, 16 );

    // Add header signature (used for verifying password).
    hmac::init( hctx, key_hmac, 32 );
    hmac::update( hctx, header, 64 );
    hmac::final( hbuf, hctx );

    memcpy( &header[64], hbuf, 32 );

    return result_e::SUCCESS;
  }
}

  Context::Context( size_t memory_,
                    double fraction_,
                    double time_,
                    const std::string& password_
                  ) :
    memory( memory_ ),
    fraction( fraction_ ),
    time( time_ ),
    password( password_.begin(), password_.end() )
  {
    assert( fraction >= 0.0 && fraction <= 0.5 );
  }

  result_e encrypt( const Context& context,
                    const std::vector<uint8_t>& vInput,
                    std::vector<uint8_t>& vOutput )
  {
    uint8_t dk[64];
    uint8_t hbuf[32];
    uint8_t header[96];
    uint8_t * key_enc = dk;
    uint8_t * key_hmac = &dk[32];
    result_e result;
    hmac::Context hctx;
    AES_KEY key_enc_exp;

    vOutput.resize( vInput.size() + 128 );

    uint8_t * outbuf = &vOutput[0];

    // Generate the header and derived key.
    if ( ( result = encrypt_params( context, header, dk ) ) != result_e::SUCCESS )
    {
      return result;
    }

    // Copy header into output buffer.
    memcpy( outbuf, header, 96 );

    // Encrypt data.
    if ( AES_set_encrypt_key( key_enc, 256, &key_enc_exp ) )
    {
      return result_e::OPENSSL_FAIL;
    }

    AES aes( &key_enc_exp, 0 );
    aes( &vInput[0], &outbuf[96], vInput.size() );

    // Add signature.
    hmac::init( hctx, key_hmac, 32 );
    hmac::update( hctx, outbuf, 96 + vInput.size() );
    hmac::final( hbuf, hctx );

    memcpy( &outbuf[96 + vInput.size()], hbuf, 32 );

    // Zero sensitive data.
    memset( dk, 0, 64 );

    return result_e::SUCCESS;
  }

  result_e decrypt( const Context& context,
                    const std::vector<uint8_t>& vInput,
                    std::vector<uint8_t>& vOutput )
  {
    uint8_t hbuf[32];
    uint8_t dk[64];
    uint8_t * key_enc = dk;
    uint8_t * key_hmac = &dk[32];
    result_e result;
    hmac::Context hctx;
    AES_KEY key_enc_exp;

    vOutput.resize( vInput.size() - 128 );

    uint8_t * outbuf = &vOutput[0];

    // All versions of the scrypt format will start with "scrypt" and
    // have at least 7 bytes of header.
    if ( ( vInput.size() < 7 ) || (
            vInput[0] != 's' ||
            vInput[1] != 'c' ||
            vInput[2] != 'r' ||
            vInput[3] != 'y' ||
            vInput[4] != 'p' ||
            vInput[5] != 't'
          ) )
    {
      return result_e::DATA_INPUT_FAIL;
    }

    // Check the format.
    if ( vInput[6] != 0 ) { return result_e::UNRECOGNIZED_FORMAT_FAIL; }

    // We must have at least 128 bytes
    if ( vInput.size() < 128 ) { return result_e::DATA_INPUT_FAIL; }

    if ( ( result = decrypt_params( context, &vInput[0], dk ) ) != result_e::SUCCESS )
    {
      return result;
    }

    // Decrypt data.
    if ( AES_set_encrypt_key( key_enc, 256, &key_enc_exp ) )
    {
      return result_e::OPENSSL_FAIL;
    }

    AES aes( &key_enc_exp, 0 );
    aes( &vInput[96], outbuf, vInput.size() - 128 );

    // Verify signature.
    hmac::init( hctx, key_hmac, 32 );
    hmac::update( hctx, &vInput[0], vInput.size() - 32 );
    hmac::final( hbuf, hctx );

    if ( memcmp( hbuf, &vInput[vInput.size() - 32], 32 ) )
    {
      return result_e::DATA_INPUT_FAIL;
    }

    // Zero sensitive data.
    memset( dk, 0, 64 );

    return result_e::SUCCESS;
  }
}
}
}
