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

#include <string>
#include <cstdint>

namespace astateful {
namespace async {
namespace net {
namespace client {
  struct Engine;
}
}
}

namespace cache {
  //!
  struct Response;

  //!
  struct Request;

  //!
  enum class result_e {
    OK,
    NO_REPLY,
    NOT_STORED,
    NOT_FOUND,
    NO_SERVER,
    UNKNOWN
  };

  //!
  struct Context final {
    //! Construct a context from the server address and the client engine which
    //! drives the context.
    //!
    Context( const async::net::client::Engine&, const std::wstring& );

    //!
    //!
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;
    Context& operator=( Context&& ) = delete;
    Context( Context&& ) = delete;

    //!
    //!
    bool Send( Response&, const Request& ) const;

    //!
    //!
    bool Send( const Request& ) const;
  private:
    //! Used for communicating with the mongo server.
    const async::net::client::Engine& m_engine;

    //! The address of the server to make the request to.
    const std::wstring m_address;
  };

  //!
  //!
  bool flush( const Context& );

  //!
  //!
  std::string get( const Context&, const std::string& );

  //!
  //!
  std::string gets( const Context&, const std::string&, uint64_t& );

  //!
  //!
  result_e del( const Context&, const std::string&, bool reply = true );

  //!
  //!
  result_e set( const Context&, const std::string&, const std::string& );

  //!
  //!
  result_e add( const Context&, const std::string&, const std::string& );

  //!
  //!
  result_e replace( const Context&, const std::string&, const std::string& );

  //!
  //!
  result_e append( const Context&, const std::string&, const std::string& );

  //!
  //!
  result_e prepend( const Context&, const std::string&, const std::string& );

  //!
  //!
  result_e cas( const Context&, const std::string&, const std::string&, uint64_t );

  //!
  //!
  std::string inc( const Context&, const std::string&, uint64_t, bool reply = true );

  //!
  //!
  std::string dec( const Context&, const std::string&, uint64_t, bool reply = true );
}
}