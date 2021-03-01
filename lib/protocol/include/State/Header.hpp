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

#include "astateful/algorithm/State.hpp"

namespace astateful {
namespace token {
namespace h1 {
  //!
  struct Context;
}
}

namespace protocol {
  //! Used to provide the request headers to other states that need it.
  template<typename U, typename V> struct StateHeader : public algorithm::State<U, V> {
    //! Build up a header format suitable for use with other states.
    //! Unfortunately we require the entire h1 context here.
    //!
    StateHeader( const token::h1::Context& h1 ) :
      algorithm::State<U, V>(),
      m_h1( h1 ) {}

    //! Return a new unknown based on the flux index as well as the values
    //! provided by the given value map.
    //!
    std::unique_ptr<U> operator()( const algorithm::value_t<U, V>&,
                                   const V& ) const override;
  private:
    //! The machine context used to store parsed HTTP data.
    const token::h1::Context& m_h1;
  };
}
}
