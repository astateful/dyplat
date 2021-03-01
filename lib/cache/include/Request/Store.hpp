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

#include "../Request.hpp"

namespace astateful {
namespace cache {
  //! Create a storage request to send to the cache server.
  struct RequestStore final : public Request {
    //! Construct the request using the various parameters here.
    //!
    //! @param The key referring to the data to either increment or decrement.
    //! @param type Can be add, set, replace, append, prepend, or cas.
    //! @param flag Any additional flags to send with this request.
    //! @param expiry The expiration date in seconds of the key.
    //! @param data The data to store under the provided key.
    //! @param cas Whether the caller wants to use cas.
    //! @param reply Whether the caller would like acknowledgement of request.
    //!
    RequestStore( const std::string& key,
                  const std::string& type,
                  int flag,
                  int expiry,
                  const std::string& data,
                  uint64_t cas,
                  bool reply );
  };
}
}