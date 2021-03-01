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

#include "Address.hpp"

namespace astateful {
namespace async {
namespace net {
  //! This socket class is suitable for creating either client or server sockets.
  //! It is never meant to be extended: this was the case in the past but the
  //! design didnt really work; there ended up being too much non socket specific
  //! code making its way into this class.
  struct Socket final {
    //! Avoid creating an invalid socket.
    //!
    Socket() = delete;

    //! 
    //!
    Socket( SOCKET&&, const SOCKADDR_STORAGE& );

    //!
    //!
    Socket( const ADDRINFOW * );

    //!
    //!
    Socket( const Socket& ) = delete;
    Socket& operator=( const Socket& ) = delete;
    Socket& operator=(Socket &&) = delete;

    //!
    //!
    Socket( Socket&& );

    //!
    //!
    operator const SOCKET&() const;

    //! Create an IOCP using the inputted handle which is the existing IOCP
    //! and the underlying socket. Interesting is that the instance of this
    //! class is the completion key, so when retrieving a queued completion
    //! status, be sure to cast the value of the key to this class type.
    //!
    //! @param iocp The existing IOCP to attach the new IOCP to.
    //! 
    void createIOCP( HANDLE iocp );

    //!
    //!
    operator const Address&() const;

    //! Free the socket. If an error occurs in freeing a socket, print to
    //! standard output so that there is some way the client knows what is
    //! happening.
    //!
    ~Socket();
  private:
    //!
    SOCKET m_socket;

    //!
    Address m_address;
  };
}
}
}
