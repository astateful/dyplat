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
  //!
  struct Socket;

  //!
  struct Address;

namespace server {
  //!
  struct Connection;

  //! Define a type for our connection generating function.
  using generator_t = std::function<Connection* (
    const Address&,
    const Address&)>;

  //! Define a managed type for our connection container.
  using connection_t = std::unordered_map<std::wstring, generator_t>;

  //! The engine class creates an instantiation of the server which can then be
  //! embedded as a part of something greater. This base engine performs
  //! thread management and socket listener initialization.
  struct Engine : public net::Engine {
    //! As a precaution, ensure that this class cannot be default constructed.
    //!
    Engine() = delete;

    //! The engine needs the connection generating function pointers. From
    //! these listeners can be created which then generate the specific
    //! connection needed to handle a connection request on that port.
    //!
    Engine( const pipe::client::Engine&, const connection_t& );

    //! This class never needs to be copied or moved since an engine represents
    //! a unique server instance that runs once per application instance.
    //!
    Engine( const Engine& ) = delete;
    Engine& operator=( const Engine& ) = delete;
    Engine& operator=( Engine&& ) = delete;
    Engine( Engine&& ) = delete;

    //! Some clients can only support IPv4 so in some test cases we run in IPv4.
    //! This should only be used for testing with some old clients. Return false
    //! if something went wrong.
    //!
    bool runIPv4();

    //! The obvious choice for servers these days is IPv6. Same return value as
    // in IPv4.
    //!
    bool runIPv6();

    //!
    //!
    void kill();

    //! The destructor will shutdown the server instance by first freeing all
    //! listeners, then calling WSACleanup(). It is important to free the
    //! listeners first, or else undefined behavior could occur.
    //!
    ~Engine();
  private:
    //! A list of instantiated sockets listening on a specific address.
    std::vector<std::unique_ptr<Socket>> m_socket;

    //! The mapping between listener and its connection handler.
    const connection_t& m_connection;

    //!
    bool m_alive;

    //! A list of producer threads used to create data.
    std::vector<std::thread> m_produce;

    //! Provide the actual run implementation that chooses the socket family
    //! based on the inputted parameter.
    //!
    bool run( int family );
  };
}
}
}
}
