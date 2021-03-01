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

#include <memory>
#include <string>

namespace astateful {
namespace plugin {
  struct Store;
  template<typename V> struct Key;

  //! Provide a transformation layer which abstracts any pre and post
  //! processing to perform on the data such as encryption and decryption
  //! or compression (or both!?)
  template<typename U> struct Pipe {
    //! Construct a pipe using the store which is the specific storage
    //! to read from and write to.
    //!
    //! @param store The storage used to abstract how the data is stored.
    //!
    Pipe( const Store& store ) : m_store( store ) {}
    virtual ~Pipe() = default;

    //! Return data read in from a storage. The key is the identifier
    //! used to retrieve the data from the storage.
    //!
    virtual std::unique_ptr<U> Read( const Key<std::string>& ) const;

    //! Output the given data to a storage. The key is the identifier
    //! used to identify the data when writing to storage.
    //!
    virtual bool Write( const Key<std::string>&,
                        const std::unique_ptr<U>& ) const;
  protected:
    //! The storage used to abstract how the data is stored.
    const Store& m_store;
  };
}
}
