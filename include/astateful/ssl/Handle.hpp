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

#include <vector>
#include <cstdint>
#include <functional>

struct ssl_st;
struct bio_st;
struct ssl_ctx_st;

namespace astateful {
namespace ssl {
  //!
  struct Handle {
    //!
    //!
    Handle( ssl_ctx_st *, int );
    virtual ~Handle();

    //! Support an interface for writing a string. This interface is only
    //! really used for testing since it is easier to work with strings when
    //! direct comparisons are needed.
    //!
    //! @param input The input string to write to the SSL buffer.
    //! @return > 0 is successful, <= 0 needs further investigation.
    //!
    int write( const std::string& input );

    //!
    //!
    int write( const std::vector<uint8_t>& );

    //! Support an interface for reading into a string. This interface is only
    //! really used for testing since it is easier to work with strings when
    //! direct comparisons are needed.
    //!
    //! @param output The output string to populate with the read data.
    //! @return > 0 is successful, <= 0 needs further investigation.
    //!
    int read( std::string& output );

    //!
    //!
    int read( std::vector<uint8_t>& );

    //!
    //!
    int shutdownState() const;

    //!
    //!
    int geterror( int );

    int ssl_handshake_finished();

    //!
    //!
    int shutdown();

    //!
    //!
    bool update( const std::vector<uint8_t>& );

    //!
    //!
    int flush( std::vector<uint8_t>& );

    //!
    //!
    const char * state();
  protected:
    //!
    ssl_st * m_ssl;

    //!
    bio_st * m_in;

    //!
    bio_st * m_out;
  private:
    //!
    const int m_max_pending;
  };
}
}
