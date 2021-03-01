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

#include "Ws2tcpip.h"

namespace astateful {
namespace async {
namespace pipe {
namespace server {

  //!
  struct Connection;

  //!
  using generator_t = std::function<void( const std::vector<uint8_t>& )>;

  //! Define a managed type for our connection container.
  using callback_t = std::unordered_map<std::string, generator_t>;

  //!
  struct Engine final : public pipe::Engine {
    //!
    //!
    Engine( const callback_t& );

    //!
    //!
    bool start() override;

    //!
    //!
    void kill();

    //!
    //!
    ~Engine();
  private:
    //! This overlapped structure handles only the connection portion of
    //! asynchronous pipe requests.
    OVERLAPPED m_overlapped;

    //!
    std::vector<HANDLE> m_handle;

    //!
    std::vector<std::unique_ptr<Connection>> m_connection;

    //! A list of producer threads used to create data.
    std::vector<std::thread> m_produce;

    //
    const callback_t& m_callback;
  };
}
}
}
}