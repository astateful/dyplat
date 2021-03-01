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

#include <string>
#include <unordered_map>

namespace astateful {
namespace plugin {
  //! Query string pairs which are parsed out of the request.
  using query_t = std::unordered_map<std::string, std::string>;

  //! Tokenize key value pairs delimited by '&' into a map whose keys and values
  //! correspond to the key value pairings in string. Previously we had inherited
  //! this class from a base Sequence class however is is unique enough that it
  //! shares no properties with other sequence classes, therefore this class
  //! stands alone.
  struct Query final {
    //! We take a string as input and not a stringstream because the
    //! input string should have been run through the percent decoder
    //! FSM first.
    //!
    explicit Query( const std::string& );

    //! This class never needs to be copied or moved since it never needs
    //! to be stored!
    //!
    Query( const Query& ) = delete;
    Query& operator=( const Query& ) = delete;
    Query& operator=( Query && ) = delete;
    Query( Query&& ) = delete;

    //! Populate the result map with what was parsed from the input
    //! source.
    //!
    bool operator()( query_t& ) const;
  private:
    //! The constant input containing the characters to parse.
    const std::string& m_input;
  };
}
}