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
namespace h2 {
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
    bool create( std::vector<uint8_t>& output ) override {

      /*union convert {
          int i;
          unsigned char c[3];
      };

      union convertt {
          int i;
          unsigned char c[3];
      };

      union convert15 {
          int i;
          unsigned char c[2];
      };

      union convert32 {
          int i;
          unsigned char c[4];
      };

      convert cvt;
      cvt.i = 0;

      convertt jk;
      jk.i = 4;

      convert32 klo;
      klo.i = 100;

      // 9 bytes
      //int length = 0;
      //memcpy(k.data(), &length)*/
      //Http2FrameHeader ack_settings_fh( 0x0, 0x4, 0x0, 0x0 );
      //ack_settings_fh.print();

      //convert15 kl;
      //kl.i = 3;*/

      //output = ack_settings_fh.write_to_buffer();
      /*output.push_back(0);
      output.push_back(3);
      output.push_back(0);
      output.push_back(0);
      output.push_back(0);
      output.push_back(100);

      return false;*/
      return true;
    }
  };
}
}
}
