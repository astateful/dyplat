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
#include <vector>
#include <cstdint>

namespace astateful {
namespace bson {
  struct ObjectId {
    //! Will create a non empty auto generated $oid suitable for use
    //! with mongodb.
    //!
    ObjectId();

    //! Construct one from a string. Useful for testing.
    //!
    ObjectId( const std::string& );

    //! The iterator uses this constructor since it has only the raw
    //! data when iterating over BSON binary data.
    //!
    ObjectId( const std::vector<uint8_t>& );

    //!
    //!
    operator std::vector<uint8_t>() const { return m_data; }

    //!
    //!
    operator std::string() const;
  private:
    //!
    std::vector<uint8_t> m_data;

    //! Used in order to provide a counter for generated object ids,
    //! thus ensuring they are somewhat unique.
    static int counter;
  };
}
}
