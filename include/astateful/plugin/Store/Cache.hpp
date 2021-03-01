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
namespace cache {
  struct Context;
}

namespace plugin {
  //! Implement a storage layer which uses memcache to persist data.
  struct StoreCache : public Store {
    //! Construct a cache store. This store will communicate with the cache
    //! server which stores the cache data.
    //!
    StoreCache( const cache::Context& );

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
    //! The context used for communicating with the cache server.
    const cache::Context& m_cache;
  };
}
}
