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

#include "../Engine.hpp"

#include <unordered_map>
#include <memory>

namespace astateful {
namespace async {
namespace net {
namespace client {
  //!
  struct Response;

  //!
  struct Request;

  //!
  struct Connection;

  //! Define a type for our connection generating function.
  using generator_t = std::function<Connection* (
    Response *,
    const Request& )>;

  //! Define a managed type for our connection container.
  using connection_t = std::unordered_map<std::wstring, generator_t>;

  //!
  struct Engine final : public net::Engine {
    //! As a precaution, ensure that this class cannot be default constructed.
    //!
    Engine() = delete;

    //!
    //!
    Engine( const pipe::client::Engine&, const connection_t& );

    //! This class never needs to be copied or moved since an engine represents
    //! a unique client instance that runs once per application instance.
    //!
    Engine( const Engine& ) = delete;
    Engine& operator=( const Engine& ) = delete;
    Engine& operator=( Engine&& ) = delete;
    Engine( Engine&& ) = delete;

    //! Run the engine with IPv4 connectivity. In the case of the client, IPv4
    //! makes the most sense when connecting to servers we have no control over.
    //! Returns false if the server failed to start for some reason.
    //!
    bool runIPv4();

    //!
    //!
    bool Send( const std::wstring&, const Request&, Response* = nullptr ) const;
  private:
    //! The mapping between listener and its connection handler.
    const connection_t& m_connection;
    
    //! The default address family to assign newly created connections.
    const int m_family;
  };
}
}
}
}
