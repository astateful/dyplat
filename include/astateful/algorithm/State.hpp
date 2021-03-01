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

#include "Value.hpp"

namespace astateful {
namespace algorithm {
  //! The main goal of the state is to output data given an inputted flux
  //! as well as some initial data that is determined by the client at
  //! construction.
  template<typename U, typename V> struct State {
    //! Default construct this class.
    //!
    State() = default;
    virtual ~State() = default;

    //! A state never needs to be copied or moved since the idea of a state is
    //! that it is completely independent of other states, and so there would
    //! be no common properties between any two states.
    //!
    State( const State& ) = delete;
    State& operator=( const State& ) = delete;
    State& operator=( State && ) = delete;
    State( State&& ) = delete;

    //! Return a new unknown based on the flux index as well as the values
    //! provided by the given value map.
    //!
    virtual std::unique_ptr<U> operator()( const value_t<U, V>&,
                                           const V& ) const = 0;
  };

  //! Define a managed type for our state container.
  template<typename U, typename V> using state_t = std::unordered_map<V, std::unique_ptr<State<U, V>>>;
}
}
