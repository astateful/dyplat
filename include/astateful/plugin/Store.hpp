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
namespace plugin {
  //! Provide enumeration definitions for testing the return type of values
  //! which populate themselves via external storage.
  enum class store_e {
    STORAGE_BAD,
    STORAGE_NOEXIST,
    DATA_EMPTY,
    DATA_BAD,
    SUCCESS,
  };

  template<typename V> struct Key;

  //! Implement a storage layer which can abstract data storage across a
  //! filesystem, network, etc.
  struct Store {
    //! Default construct this class.
    //!
    Store() = default;
    virtual ~Store() = default;

    //! Populate the given buffer with data read in from storage, using the
    //! key to locate the data to read.
    //!
    virtual store_e Get( const Key<std::string>&,
                         std::vector<uint8_t>& ) const = 0;

    //! Output the given buffer to storage, using the key to to define where
    //! to save the data to in the storage.
    //!
    virtual store_e Set( const Key<std::string>&,
                         const std::vector<uint8_t>& ) const = 0;
  };
}
}
