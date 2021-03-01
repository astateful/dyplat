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

namespace astateful {
namespace algorithm {
  //!
  template<typename U, typename V> struct Context;
}

namespace protocol {
  //!
  template<typename U, typename V> struct Transform;

  //!
  //!
  template<typename U, typename V> struct Request {
    //!
    //!
    Request() = default;
    virtual ~Request() = default;

    //! Given a buffer of unknown size, use the input number of bytes to
    //! examine only the most recent chunk of data in the buffer. Then,
    //! based on the saved FSM state and this chunk, determine if we have
    //! finished reading all the data.
    //!
    //! @param data A buffer whose size may not equal the input length.
    //! @param length The number of bytes that we read from the data.
    //! @return True if the FSM produces a valid transition state.
    //!
    virtual bool update( const uint8_t *, int ) = 0;

    //!
    //!
    bool update( const char * data, int length) {
      return update( reinterpret_cast<const uint8_t*>( data ), length );
    }

    //!
    //!
    bool update( std::string& data ) {
      return update( data.data(), static_cast<int>( data.length() ) );
    }

    //!
    //!
    virtual V hash() const = 0;

    //!
    //!
    virtual void clear() = 0;

    //!
    //!
    virtual std::string upgradeProtocol() { return ""; }

    //! Provide a way for a client to choose which executing state
    //! to select based on data provided by the context.
    //!
    virtual bool initAlgorithm( algorithm::Context<U,V>& ) const = 0;
  };
}
}
