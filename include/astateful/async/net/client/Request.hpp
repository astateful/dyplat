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

#include <vector>
#include <cstdint>

namespace astateful {
namespace async {
namespace net {
namespace client {
  //! The request is basically used to create a raw buffer which is sent to the
  //! server. The request can be initiaized with a buffer to send or clients can
  //! extend this class and populate the buffer themselves.
  struct Request {
    //! Default construct this class.
    //!
    Request() = default;
    virtual ~Request() = default;

    //! Construct this class with a raw buffer. This is commonly used by clients
    //! to rapidly initialize this class with a buffer instead of building one.
    //! This buffer will be sent directly over the wire.
    //!
    //! @param buffer The buffer to populate this class with.
    //!
    Request( const std::vector<uint8_t>& buffer );

    //! Allow the engine to directly extract the raw buffer in order to send it
    //! over the wire.
    //!
    //! @return The raw buffer to send over the wire.
    //!
    const std::vector<uint8_t>& buffer() const;
  protected:
    //! The buffer containing the current data to send or receive.
    std::vector<uint8_t> m_buffer;
  };
}
}
}
}
