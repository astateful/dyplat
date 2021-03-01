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

#include <memory>
#include <string>

namespace astateful {
namespace bson {
  struct Serialize;
  enum class error_e;
}

namespace mongo {
  //!
  struct Context;

  //!
  struct Response;

  using namespace astateful::bson;

  //!
  struct Iterate : public std::iterator<std::input_iterator_tag, Serialize> {
    enum class option_e {
      TAILABLE = ( 1 << 1 ), // Create a tailable cursor.
      SLAVE_OK = ( 1 << 2 ), // Allow queries on a non-primary node.
      NO_CURSOR_TIMEOUT = ( 1 << 4 ), // Disable cursor timeouts.
      AWAIT_DATA = ( 1 << 5 ), // Momentarily block for more data.
      EXHAUST = ( 1 << 6 ), // Stream in multiple 'more' packages.
      PARTIAL = ( 1 << 7 ) // Allow reads even if a shard is down.
    };

    enum class Error {
      EXHAUSTED, // The cursor has no more results.
      INVALID, // The cursor has timed out or is not recognized.
      PENDING, // Tailable cursor still alive but no data.
      QUERY_FAIL, // The server returned an '$err' object, indicating query failure.
      BSON_ERROR // Something is wrong with the BSON provided.
    };

    //!
    //!
    Iterate( const Context&,
             const std::string&,
             const Serialize&,
             const int,
             const int,
             const int,
             bson::error_e&,
             Serialize * = nullptr );

    Iterate& operator=( const Iterate& ) = delete;

    //! pre increment operator
    //!
    Iterate& operator++();

    //! Determine the validity of the iterate by examining the current
    //! serialization being pointed to.
    //!
    operator bool() const { return static_cast<bool>( m_current ); }

    //! Implement the dereference operator for the serialization that the
    //! iteration currently points to.
    //!
    Serialize& operator*() const { return *m_current; }

    //! Implement the member access operator for the serialization that the
    //! iteration currently points to.
    //!
    Serialize * operator->() const { return m_current.get(); }

    //! Required due to forward declaration of classes.
    //!
    ~Iterate();
  private:
    //! The current BSON serialization being pointed to by this iterate.
    std::unique_ptr<Serialize> m_current;

    //! The response is owned by cursor.
    std::unique_ptr<Response> m_response;

    //! //! The context used for communicating with the mongo server.
    const Context& m_context;

    //! The current position of the cursor in the result set.
    int32_t m_position;

    //! The number of documents to limit the result set by.
    const int m_limit;

    //! Number returned so far.
    int m_seen;

    //!
    const std::string m_ns;

    //! Errors on this cursor.
    Error m_error;

    //!
    //!
    bool More();
  };
}
}
