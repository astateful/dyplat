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

#include "State.hpp"
#include "Flux.hpp"

namespace astateful {
namespace algorithm {
  //! This class provides a context for an evaluation instance. An evaluation
  //! instance is typically identified by a string, the hashed value of that
  //! string, and the index of the executing state to be optimized.
  template<typename U, typename V> struct Context final {
    //! The state map which is needed to optimize the initial state.
    state_t<U, V> state = {};

    //! The shared storage map containing our computed fluxes.
    value_t<U, V> value = {};

    //! The flux map which controls the state evaluation order.
    flux_t<V> flux = {};

    //! The index of the executing state that is being optimized.
    V exec = {};

    //! Default construct this class.
    //!
    Context() = default;

    //! This class never needs to be copied or moved since it is meant to
    //! uniquely represent an evaluation instance.
    //!
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;
    Context& operator=( Context&& ) = delete;
    Context( Context&& ) = delete;
  };
}
}
