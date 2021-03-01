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
#include <cassert>
#include <unordered_map>

namespace astateful {
namespace token {
namespace h1 {
  //!
  enum class state_e;

  //!
  struct Context {
    //! The raw HTTP version.
    std::string version;

    //! The maximum number of query string pairs to allow.
    const static int maxNumQuery = 20;

    //! Query string values that are parsed out of a request.
    std::vector<std::pair<std::string,std::string>> query;

    //! The request method (GET, POST, etc.)
    std::string method;

    //! The URL-decoded URI.
    std::string uri;

    //! The maximum number of header pairs to allow.
    const static int maxNumHeader = 20;

    //! The headers parsed from the HTTP request.
    std::vector<std::pair<std::string,std::string>> header;

    //! The current state of the context.
    state_e state;

    //! The body parsed out of a POST request.
    std::string body;

    //! A secondary storage buffer to handle percent encodings.
    std::string buffer;

    //!
    //!
    bool headerAt( const std::string& key ) const;

    //!
    //!
    const std::string& headerValue( const std::string& key) const;

    //!
    //!
    std::string rawId() const;

    //!
    //!
    Context( state_e );
  };
}
}
}