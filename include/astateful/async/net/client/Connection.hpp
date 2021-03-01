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
namespace net {
namespace client {
  //!
  struct Response;

  //!
  struct Request;

  //!
  struct Connection final : public net::Connection {
    //! Provide the connection with the response and request objects.
    //! The response object is not owned by the connection and the request
    //! object is not stored; we only need it in order to copy the data
    //! to send.
    //!
    //! @param response The response object used to handle recieved data.
    //! @param request The request object used to send data.
    //!
    Connection( Response * response, const Request& request );
  protected:
    //!
    //!
    bool recvEvent( DWORD, const pipe::client::Engine& ) override;

    //!
    //!
    bool sendEvent( DWORD, const pipe::client::Engine& ) override;
  private:
    //! The object used to provide a response for this connection.
    Response * m_response;
  };
}
}
}
}
