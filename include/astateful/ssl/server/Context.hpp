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

#include "../Context.hpp"

namespace astateful {
namespace ssl {
namespace server {
  //!
  struct Context final : public ssl::Context {
    //!
    //!
    struct tlsextalpnctx_st {
      unsigned char *data;
      unsigned short len;

      tlsextalpnctx_st();
    };

    //!
    //!
    Context( const std::wstring&,
             const std::wstring&,
             const std::string& );

    //! Retrieve the negotiated ALPN, used to pass on to other
    //! protocol selection methods.
    //!
    const std::string& alpn();

    //!
    //!
    ~Context();
  private:
    //! Stored as a string format since the actual ALPN context
    //! has the protocol length stored in first byte.
    const std::string m_alpn;

    //!
    tlsextalpnctx_st m_alpn_ctx;
  };
}
}
}
