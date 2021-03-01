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

#include <functional>
#include <unordered_map>
#include <memory>

namespace astateful {
namespace async {
namespace net {
  //!
  struct Address;

namespace server {
  //!
  struct Response;

  //!
  struct Request;

  //! The connection class handles data on a per IO basis, meaning that it has
  //! all the information it needs in order to process a completion. packet. This
  //! information includes buffered data as well as the overlapped structure that
  //! we pass into WSASend and WSARecv calls.
  struct Connection : public net::Connection {
    //! As a precaution, ensure that this class cannot be default constructed.
    //!
    Connection() = delete;

    //! While the connection deals with data on a socket level, the response
    //! is used to provide data to send back to the client. This
    //! separation was done to increase flexibility through composition
    //! and thus avoid the use of multiple inheritance. For example, we
    //! can have an SSL connection type and the response does not have to know
    //! about it; it just has to worry about sending back the data.
    //!
    Connection( const Address&, const Address& );

    //! This class never needs to be copied or moved because it is unique
    //! for a single connection only; its properties can in no way ever
    //! apply to other connections.
    //!
    Connection( const Connection& ) = delete;
    Connection& operator=( const Connection& ) = delete;
    Connection& operator=( Connection && ) = delete;
    Connection( Connection&& ) = delete;
  protected:
    //! The local addresses from which the connection was created.
    const Address& m_local;

    //! The remote address which has connected to the local address.
    const Address& m_remote;
  };
}
}
}
}
