//! Copyright 2009 Colin Percival
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//! 1. Redistributions of source code must retain the above copyright
//!    notice, this list of conditions and the following disclaimer.
//! 2. Redistributions in binary form must reproduce the above copyright
//!    notice, this list of conditions and the following disclaimer in the
//!    documentation and/or other materials provided with the distribution.
//!
//! THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//! ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//! FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//! DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//! OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//! HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//! LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//! OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//! SUCH DAMAGE.
//!
//! This file was originally written by Colin Percival as part of the Tarsnap
//! online backup system.
//!
//! The MIT License (MIT)
//!
//! Copyright (c) 2014 Thomas Kovacs
//!
//! Permission is hereby granted, free of charge, to any person obtaining a
//! copy of this software and associated documentation files (the "Software"),
//! to deal in the Software without restriction, including without limitation
//! the rights to use, copy, modify, merge, publish, distribute, sublicense,
//! and / or sell copies of the Software, and to permit persons to whom the
//! Software is furnished to do so, subject to the following conditions :
//!
//! The above copyright notice and this permission notice shall be included in
//! all copies or substantial portions of the Software.
//!
//! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//! FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//! DEALINGS IN THE SOFTWARE.
#pragma once

#include <cstdint>

namespace astateful
{
  namespace crypto
  {
    static __inline uint32_t be32dec ( const void *pp )
    {
      const uint8_t * p = static_cast <uint8_t const *> ( pp );

      return ( static_cast <uint32_t> ( p [3] ) +
               ( static_cast <uint32_t> ( p [2] ) << 8 ) +
               ( static_cast <uint32_t> ( p [1] ) << 16 ) +
               ( static_cast <uint32_t> ( p [0] ) << 24 ) );
    };

    static __inline void be32enc ( void *pp, uint32_t x )
    {
      uint8_t * p = static_cast <uint8_t *> ( pp );

      p[3] = x & 0xff;
      p[2] = ( x >> 8 ) & 0xff;
      p[1] = ( x >> 16 ) & 0xff;
      p[0] = ( x >> 24 ) & 0xff;
    };

    static __inline uint64_t be64dec ( const void *pp )
    {
      const uint8_t *p = static_cast <uint8_t const *> ( pp );

      return ( ( uint64_t )( p[7] ) + ( ( uint64_t )( p[6] ) << 8 ) +
               ( ( uint64_t )( p[5] ) << 16 ) + ( ( uint64_t )( p[4] ) << 24 ) +
               ( ( uint64_t )( p[3] ) << 32 ) + ( ( uint64_t )( p[2] ) << 40 ) +
               ( ( uint64_t )( p[1] ) << 48 ) + ( ( uint64_t )( p[0] ) << 56 ) );
    };

    static __inline void be64enc ( void *pp, uint64_t x )
    {
      uint8_t * p = static_cast <uint8_t *> ( pp );

      p[7] = x & 0xff;
      p[6] = ( x >> 8 ) & 0xff;
      p[5] = ( x >> 16 ) & 0xff;
      p[4] = ( x >> 24 ) & 0xff;
      p[3] = ( x >> 32 ) & 0xff;
      p[2] = ( x >> 40 ) & 0xff;
      p[1] = ( x >> 48 ) & 0xff;
      p[0] = ( x >> 56 ) & 0xff;
    };

    static __inline uint32_t le32dec ( const void *pp )
    {
      const uint8_t *p = static_cast <uint8_t const *> ( pp );

      return ( ( uint32_t )( p[0] ) + ( ( uint32_t )( p[1] ) << 8 ) +
               ( ( uint32_t )( p[2] ) << 16 ) + ( ( uint32_t )( p[3] ) << 24 ) );
    };

    static __inline void le32enc ( void *pp, uint32_t x )
    {
      uint8_t * p = static_cast <uint8_t *> ( pp );

      p[0] = x & 0xff;
      p[1] = ( x >> 8 ) & 0xff;
      p[2] = ( x >> 16 ) & 0xff;
      p[3] = ( x >> 24 ) & 0xff;
    };

    static __inline uint64_t le64dec ( const void *pp )
    {
      const uint8_t *p = static_cast <uint8_t const *> ( pp );

      return ( ( uint64_t )( p[0] ) + ( ( uint64_t )( p[1] ) << 8 ) +
               ( ( uint64_t )( p[2] ) << 16 ) + ( ( uint64_t )( p[3] ) << 24 ) +
               ( ( uint64_t )( p[4] ) << 32 ) + ( ( uint64_t )( p[5] ) << 40 ) +
               ( ( uint64_t )( p[6] ) << 48 ) + ( ( uint64_t )( p[7] ) << 56 ) );
    };

    static __inline void le64enc ( void *pp, uint64_t x )
    {
      uint8_t * p = static_cast <uint8_t *> ( pp );

      p[0] = x & 0xff;
      p[1] = ( x >> 8 ) & 0xff;
      p[2] = ( x >> 16 ) & 0xff;
      p[3] = ( x >> 24 ) & 0xff;
      p[4] = ( x >> 32 ) & 0xff;
      p[5] = ( x >> 40 ) & 0xff;
      p[6] = ( x >> 48 ) & 0xff;
      p[7] = ( x >> 56 ) & 0xff;
    };
  };
};
