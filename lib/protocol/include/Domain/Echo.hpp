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

#include "../Domain.hpp"
#include "../State/Upgrade.hpp"
#include "../State/Protocol.hpp"

namespace astateful {
namespace protocol {
  //! The echo domain is used, in a h1 context, to fulfill an upgrade request
  //! to a WebSocket domain of the same name.
  template<typename U, typename V = std::string> struct DomainEcho :
    protocol::Domain< U> {
    //!
    //!
    DomainEcho( algorithm::Context<U, V>& algorithm ) :
      Domain<U>( algorithm ) {}
  protected:
    //!
    //!
    bool h1( const token::h1::Context& ) override {
      m_algorithm.exec = "echo";
      m_algorithm.state["echo"] = std::make_unique<StateUpgrade<U, V>>();
      m_algorithm.state["protocol"] = std::make_unique<StateProtocol<U, V>>();

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "echo" ),
                                std::forward_as_tuple( "method" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "echo" ),
                                std::forward_as_tuple( "ws" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "protocol" ),
                                std::forward_as_tuple( "ws",
                                std::vector<std::string>{ "header", "method" } ) );

      return true;
    }
  };
}
}