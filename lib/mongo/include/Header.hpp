//! Copyright 2009-2012 10gen Inc.
//!
//! Licensed under the Apache License, Version 2.0 (the "License"); you may not
//! use this file except in compliance with the License. You may obtain a copy
//! of the License at
//!
//! http://www.apache.org/licenses/LICENSE-2.0
//!
//! Unless required by applicable law or agreed to in writing, software
//! distributed under the License is distributed on an "AS IS" BASIS,
//! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//! See the License for the specific language governing permissions and
//! limitations under the License.
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
namespace mongo {
  //!
  enum class op_e : int {
    MSG = 1000,
    UPDATE = 2001,
    INSERT = 2002,
    QUERY = 2004,
    GET_MORE = 2005,
    DEL = 2006,
    KILL_CURSORS = 2007
  };

  //!
  struct Header {
    //!
    //!
    Header() = delete;

    //!
    //!
    Header( int, int, op_e );

    //!
    //!
    explicit Header( const std::vector<uint8_t>& );

    //!
    //!
    Header( const Header& ) = delete;
    Header& operator=( const Header& ) = delete;
    Header& operator=(Header&&) = delete;
    Header( Header&& ) = delete;

    //! Pack the internal data into a series of bytes that can be
    //! sent over the wire to the mongo server.
    //!
    std::vector<uint8_t> pack( size_t );
  private:
    //!
    int32_t m_id;

    //!
    int32_t m_response;

    //!
    int32_t m_op;
  };
}
}
