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

#include "astateful/async/net/client/Response.hpp"

#include <memory>

#include "Header.hpp"

namespace astateful {
namespace bson {
  struct Serialize;
}

namespace mongo {
  using namespace astateful::bson;

  //!
  struct Response final : public async::net::client::Response {
    //! Default construct this class.
    //!
    Response() = default;

    //!
    //!
    Response( const Response& ) = delete;
    Response& operator=( const Response& ) = delete;
    Response& operator=( Response&& ) = delete;
    Response( Response&& ) = delete;

    //! Parse the input buffer according to the mongodb response spec. First
	//! the size is parsed, then the header, then the fields, then the body.
	//!
	//! @param data The data buffer containing the raw response.
	//! @param length The length of the data buffer.
	//! @return True if the response is finished, false otherwise.
    //!
    bool Accept( const char * data, int length ) override;

    //! Return the number of BSON documents that this class is currently
    //! storing.
    //!
	int count() const { return m_count; }

    //! Return the id of this reply. This id is needed when making an
    //! additional query for more data.
    //!
	int64_t id() const { return m_id; }

    //! Return the current size of all BSON documents stored in this class.
    //!
	size_t size() const { return m_buffer.size(); }

    //! Create a serialization object from the input position. The input
	//! position is a position in the buffered result set that the caller
	//! stores. It is assumed to point to a valid serialization. The position
	//! is incremented by the size of the new serialization.
	//!
	//! @param position The position from which to create a serialization from.
	//! @return A new serialization object, and the position is incremented.
    //!
    std::unique_ptr<Serialize> GenerateSerialize( int32_t& position ) const;
  private:
    //! Enumerate the various states the response can be in.
    enum class state_e {
      SIZE,
      HEADER,
      FIELD,
      BODY
    };

    //! Store the current state of this response object.
    state_e m_state = state_e::SIZE;

    //! A temporary buffer holding some data to be parsed.
	std::vector<uint8_t> m_buffer = {};

    //!
    int32_t m_size = 0;

    //!
    int32_t m_flag = 0;

    //!
    int32_t m_start = 0;

    //!
    int32_t m_count = 0;

    //! The id of the iterate that originated from the query.
    int64_t m_id = 0;

    //!
    std::unique_ptr<Header> m_header = nullptr;
  };
}
}
