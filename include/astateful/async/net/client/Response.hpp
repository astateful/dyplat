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
namespace async {
namespace net {
namespace client {
  //! A client response needs to have some internal state to let the caller
  //! know when the response is finished, in the case that the caller needs
  //! to wait on the response. This internal state is very hackish since
  //! typically it is modified from another thread.
  struct Response {
    //! Default construct this class.
    //!
    Response() = default;
    virtual ~Response() = default;

    //! Make this class non-copyable and non-movable because of the volatility
    //! of the internal state, which is typically modified by another thread.
    //!
    Response( const Response& ) = delete;
    Response& operator=( const Response& ) = delete;
    Response& operator=( Response&& ) = delete;
    Response( Response&& ) = delete;

    //!
    //!
    virtual bool Accept( const char *, int ) = 0;

    //! Provide a way for someone to set whether the response is finished or
    //! not. Typically another thread will be setting this on the response
    //! object.
    //!
    //! @param done Setting true indicates that the response is ready.
    //!
    void setDone( bool done );

    //! Returns true if the response is ready and the caller is able to
    //! obtain valid data.
    //!
    operator bool() const;
  protected:
    //! An internal state which is updated whenever the response is valid.
    bool m_done = false;
  };
}
}
}
}
