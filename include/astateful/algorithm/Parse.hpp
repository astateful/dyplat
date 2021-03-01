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

#include "Persist.hpp"

namespace astateful {
namespace algorithm {
  //! The parse approach uses log files built from one of the other three
  //! generation evaluators to dynamically create the states and memoizations and
  //! then populate the states in the correct order. A definite speed advantage
  //! over the generation evaluators since we should be able to process the
  //! states in O(n) time.
  template<typename U, typename V> struct Parse final {
    //! Construct a parse with the persistence layer used to determine if
    //! a value needs to be computed or not.
    //!
    //! @param persist The persistence layer to use when looking up a value.
    //!
    Parse( const Persist<U, V>& persist ) : m_persist( persist ) {}

    //! Since the correct flux computation order has already been computed,
    //! we can "blindly" pull but using the correct order, thus properly
    //! satisfying all dependencies for the initial state.
    //!
    //! @param context The context representing an evaluation instance.
    //!
    void operator()( Context<U, V>& context ) const {
      for ( auto& value : context.value ) {
        value.second.SetData( m_persist( context, value.second.create(), value.first ) );
      }
    }
  private:
    //! The persistence layer to use when generating a value.
    const Persist<U, V>& m_persist;
  };
}
}
