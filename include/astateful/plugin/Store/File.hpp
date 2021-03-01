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

#include "../Store.hpp"

namespace astateful {
namespace plugin {
  //! Implement a storage layer which uses the filesystem to persist data.
  struct StoreFile : public Store {
    //! Initialize the file storage with the root directory under which
    //! all prefixes will be stored, as well as the name of the
    //! memoization file to save under the prefix directory.
    //!
    //! @param path The folder path which contains all generated contexts.
    //!
    StoreFile( const std::wstring& path ) : m_path( path ), Store() {}

    //! Populate the given buffer with data read in from storage, using the
    //! key to locate the data to read.
    //!
    store_e Get( const Key<std::string>&,
                 std::vector<uint8_t>& ) const override;

    //! Output the given buffer to storage, using the key to to define where
    //! to save the data to in the storage.
    //!
    store_e Set( const Key<std::string>&,
                 const std::vector<uint8_t>& ) const override;
  private:
    //! The folder path which contains all generated contexts.
    const std::wstring m_path;
  };
}
}
