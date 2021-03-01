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

#include "astateful/async/net/client/Request.hpp"

#include <memory>

namespace astateful {
namespace bson {
  struct Serialize;
  enum class error_e;
}

namespace mongo {
  using namespace bson;

  enum class op_e;

  struct Header;

  //! This class uses a listener to send data to the mongo server. There are
  //! methods defined to convert basic times to the format needed. Note that the
  //! request length is computed dynamically from the data vector.
  struct Request : public async::net::client::Request {
    //!
    //!
    Request( int, int, op_e, int );

    //!
    //!
    Request( int, int, op_e, int, const std::string& );

    //! Required due to forward declaration of Header.
    //!
    ~Request();
  protected:
    //!
    //!
    void populate();

    //!
    //!
    bool appendSerialize( const Serialize&, error_e& );

    //!
    //!
    bool setBody( const Serialize&, int, error_e& );

    //!
    //!
    bool setBody( const std::vector<Serialize>&, int, error_e& );

    //!
    //!
    void appendInt( int32_t );

    //!
    //!
    void appendLong( int64_t );
  private:
    //!
    std::vector<uint8_t> m_data;

    //!
    std::unique_ptr<Header> m_header;

    //!
    std::vector<uint8_t> m_body;

    //!
    op_e m_op;
  };
}
}
