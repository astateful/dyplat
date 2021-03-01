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

#include "../Pipe.hpp"

#include "astateful/crypto/scrypt/Context.hpp"

namespace astateful {
namespace plugin {
  //! Extend the transformation layer to provide encryption and decryption of
  //! inputted data. Note that one should employ scrypt carefully in order to
  //! avoid slowing down the plugins too much.
  template<typename U> struct PipeScrypt: public Pipe<U> {
    //!
    //!
    //! @param password The The password used to decrypt and encrypt data.
    //! @param store The storage used to abstract how the data is stored.
    //!
    PipeScrypt( const std::string& password, const Store& store ) :
      m_encrypt( 0, 0.5, 300.0, password ),
      m_decrypt( 0, 0.5, 1500.0, password ),
      Pipe<U>( store )
    {}

    //! Return data read in from a storage. The key is the identifier
    //! used to retrieve the data from the storage.
    //!
    std::unique_ptr<U> Read( const Key<std::string>& ) const override;

    //! Output the given data to a storage. The key is the identifier
    //! used to identify the data when writing to storage.
    //!
    bool Write( const Key<std::string>&,
                const std::unique_ptr<U>& ) const override;
  private:
    //! The scrypt context to use for encrypting the data.
    const crypto::scrypt::Context m_encrypt;

    //! The scrypt context to use for decrypting the data.
    const crypto::scrypt::Context m_decrypt;
  };
}
}
