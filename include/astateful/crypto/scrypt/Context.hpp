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
#include <vector>

namespace astateful {
namespace crypto {
namespace scrypt {
  //!
  enum class result_e {
    SUCCESS,
    MEM_FAIL,
    CLOCK_FAIL,
    KEY_FAIL,
    SALT_FAIL,
    OPENSSL_FAIL,
    MALLOC_FAIL,
    DATA_INPUT_FAIL,
    UNRECOGNIZED_FORMAT_FAIL,
    DECRYPT_MEM_FAIL,
    DECRYPT_TIME_FAIL,
    PASSWORD_FAIL
  };

  //! This class represents the algorithm being used to perform the encryption
  //! and decryption.
  struct Context final {
    //! The maximum number of bytes of storage to use for V array (
    //! which is by far the largest consumer of memory).  If this value
    //! is set to 0, no maximum will be enforced; any other value less
    //! than 1 MiB will be treated as 1 MiB.
    const size_t memory;

    //! The maximum fraction of available storage to use for the V
    //! array, where "available storage" is defined as the minimum out
    //! of the RLIMIT_AS, RLIMIT_DATA. and RLIMIT_RSS resource limits (
    //! if any are set).  If this value is set to 0 or more than 0.5 it
    //! will be treated as 0.5; and this value will never cause a limit
    //! of less than 1 MiB to be enforced.
    const double fraction;

    //! The maximum amount of CPU time to spend computing the derived
    //! keys, in seconds.
    const double time;

    //!
    const std::vector<uint8_t> password;

    //!
    //!
    Context( size_t, double, double, const std::string& );
  };

  //! Encrypt inbuflen bytes from inbuf, writing the resulting
  //! inbuflen + 128 bytes to outbuf.
  //!
  result_e encrypt( const Context&,
                    const std::vector<uint8_t>&,
                    std::vector<uint8_t>& );

  //! Decrypt inbuflen bytes fro inbuf, writing the result into
  //! outbuf and the decrypted data length to outlen. The allocated
  //! length of outbuf must be at least inbuflen.
  //!
  result_e decrypt( const Context&,
                    const std::vector<uint8_t>&,
                    std::vector<uint8_t>& );
}
}
}
