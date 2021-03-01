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

#include "../Connection.hpp"

namespace astateful {
namespace async {
namespace pipe {
namespace client {
  struct Engine;
}
}

namespace net {
  //!
  struct Connection : public async::Connection {
    //!
    //!
    Connection();
    Connection( const std::vector<uint8_t>& );

    //! Update the connection buffer. Implementors need to check the
    //! current buffer along with the inputted transfer value to determine
    //! the actual amount received.
    //!
    bool update( DWORD, const pipe::client::Engine& );

    //! Transfer on the connection; the flag indicates whether a read or
    //! write operation is performed on the socket. Since we are using
    //! overlapped IO, returns false if error != ERROR_IO_PENDING.
    //!
    bool transfer( SOCKET, const pipe::client::Engine& );
  protected:
    //!
    enum class flag_e {
      RECV,
      SEND
    };

    //! A WSA data buffer provides additional buffering functionality.
    WSABUF m_wsa_buf;

    //! Used to indicate read or write status for the socket.
    flag_e m_flag;

    //!
    //!
    virtual bool recvEvent( DWORD, const pipe::client::Engine& ) = 0;

    //!
    //!
    virtual bool sendEvent( DWORD, const pipe::client::Engine& ) = 0;
  private:
    //! Initiate a receive operation on the given socket.
    //!
    bool recv( SOCKET );

    //! Initiate a send operation on the given socket.
    //!
    bool send( SOCKET );
  };
}
}
}
