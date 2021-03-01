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
namespace byte {
namespace h2 {
  //!
  struct FrameHeader {
    FrameHeader();

    FrameHeader( uint32_t l, uint8_t t, uint8_t f, uint32_t sid );
  
    FrameHeader( const uint8_t* buffer, size_t length );

    void read_from_buffer(const uint8_t* buffer, size_t buflen);

    std::vector<uint8_t> write_to_buffer();

    uint32_t get_length();
    uint8_t get_type();
    uint8_t get_flags();
    uint32_t get_stream_id();

  private:
    uint32_t length;
    uint8_t type;
    uint8_t flags;
    uint32_t stream_id;
  };
}
}
}