#pragma once

#include <cstdint>

namespace astateful
{
  namespace crypto
  {
    class SMIX
    {
      //!
      //!
      void blkcpy ( void *, void *, size_t );

      //!
      //!
      void blkxor ( void *, void *, size_t );

      //! Apply the salsa20/8 core to the provided block.
      //!
      void salsa20_8 ( uint32_t [16] );

      //! Compute Bout = BlockMix_{ salsa20 / 8, r }(Bin). The input Bin
      //! must be 128r bytes in length; the output Bout must also be the
      //! same size. The temporary space X must be 64 bytes.
      //!
      void blockmix_salsa8 ( uint32_t *, uint32_t *, uint32_t *, size_t );

      //! Return the result of parsing B_{2r-1} as a little-endian integer.
      //!
      uint64_t integerify ( void *, size_t );
    public:
      //! Compute B = SMix_r (B, N).The input B must be 128r bytes in
      //! length; the temporary storage V must be 128rN bytes in length;
      //! the temporary storage XY must be 256r + 64 bytes in length. The
      //! value N must be a power of 2 greater than 1. The arrays B, V,
      //! and XY must be aligned to a multiple of 64 bytes.
      //!
      void operator () ( uint8_t *, size_t, uint64_t, uint32_t *, uint32_t * );
    };
  }
}
