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

#include "../Response.hpp"

namespace astateful {
namespace protocol {
namespace h1 {
  //!
  //!
  template<typename U, typename V> struct Response : 
    public protocol::Response<U, V> {
    //!
    //!
    Response( const plugin::Pipe<U>& pipe,
              const plugin::Compress<U, V>& compress,
              const plugin::Expand<U, V>& expand,
              const algorithm::Generate<U, V>& generate,
              const algorithm::Parse<U, V>& parse,
              const Transform<U, V>& transform ) :
              protocol::Response<U, V>( pipe,
              compress,
              expand,
              generate,
              parse,
              transform ) {}

    //!
    //!
    void fail( std::vector<uint8_t>& output ) const override {
      protocol::Response<U, V>::fail( output );

      const std::string message( "The server encountered an error." );

      std::string response( "HTTP/1.1 500 Internal Server Error\r\n" );
      response += "Content-Length: ";
      response += std::to_string( message.length() );
      response += "\r\n\r\n";
      response += message;

      output.assign( response.begin(), response.end() );
    }
  };
}
}
}
