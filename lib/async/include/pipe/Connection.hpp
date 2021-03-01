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

#include "astateful/async/Connection.hpp"

namespace astateful {
namespace async {
namespace pipe {
  //!
  enum class flag_e {
    READ,
    WRITE,
    DONE
  };

  //!
  struct Connection : public async::Connection {
    //!
    //!
    Connection( flag_e );

    //!
    //!
    Connection( flag_e, const std::vector<uint8_t>& );

    //!
    //!
    virtual bool update( DWORD ) = 0;

    //!
    //!
    bool transfer( HANDLE );
  protected:
    //!
    flag_e m_flag;
  private:
    //!
    //!
    int write( HANDLE );

    //!
    //!
    int read( HANDLE );
  };
}
}
}
