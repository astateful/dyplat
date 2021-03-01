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

#include "astateful/async/net/client/Response.hpp"

#include <string>

namespace astateful {
namespace cache {
  //!
  enum class result_e;

  //!
  struct Response : public async::net::client::Response {
  protected:
    //! Enumerate the various states the response can be in.
    enum class state_e {
      BODY,
      END,
      VALUE,
      KEY,
      FLAG,
      CAS,
      BYTES,
      BYTES_STORED,
      DATA
    };
  public:
    //! Construct the response with the initial state used to parse the
    //! response.
    //!
    //! @param state The initial state from which response parsing begins.
    //!
    Response( state_e state );

    //! The default accept implementation will look for an \r\n to signal the
    //! end of the raw response. The buffer could only be a portion of the\
    //! actual request.
    //!
    //! @param data The data buffer containing the raw response.
    //! @param length The length of the data buffer.
    //! @return True if the response is finished, false otherwise.
    //!
    bool Accept( const char * data, int length ) override;
  protected:
    //! Store the current state of this response object.
    state_e m_state;

    //! A temporary buffer holding some data to be parsed.
    std::string m_buffer = "";
  };
}
}