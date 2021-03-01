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

#include "Context.hpp"

namespace astateful {
namespace algorithm {
  //! Provide the client with a way to integrate an IO persistence layer
  //! into the value computation.
  template<typename U, typename V> struct Persist {
    //! Default construct this class.
    //!
    Persist() = default;
    virtual ~Persist() = default;

    //! The default behavior is to always compute the value from the
    //! state directly.
    //!
    //! @param context The evaluation context containing the creator state.
    //! @param state The index of the state which creates this value.
    //! @param flux The name of the value to pass to the state when creating.
    //! @return A new value created from the state.
    //!
    virtual std::unique_ptr<U> operator()( const algorithm::Context<U, V>& context,
                                           const V& state, const V& flux ) const {
      // This check is definitely required. Do not remove.
      if ( context.state.find( state ) == end( context.state ) )
        return nullptr;

      return ( *context.state.at( state ) )( context.value, flux );
    }
  };
}
}
