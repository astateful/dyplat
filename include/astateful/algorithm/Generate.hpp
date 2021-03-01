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
  //! The generation evaluation algorithms try to optimize the initial state by
  //! satisfying all the constraints of the state, namely that all of its pulled
  //! unknowns are populated.
  template<typename U, typename V> struct Generate {
    //! Construct a generate with the persistence layer used to determine if
    //! a value needs to be computed or not.
    //!
    //! @param persist The persistence layer to use when looking up a value.
    //!
    Generate( const Persist<U, V>& persist ) : m_persist( persist ) {}
    virtual ~Generate() = default;

    //! A generate never needs to be copied or moved since it is only needed
    //! one time to compute an evaluation instance.
    //!
    Generate( const Generate& ) = delete;
    Generate& operator=( const Generate& ) = delete;
    Generate& operator=( Generate&& ) = delete;
    Generate( Generate&& ) = delete;

    //! Since the evaluator functions like an algorithm, provide a default
    //! invocation overload to make the syntax appear more natural to the
    //! callee.
    //!
    virtual void operator()( Context<U, V>&, int& ) const = 0;
  protected:
    //! The persistence layer to use when generating a value.
    const Persist<U, V>& m_persist;
  };
}
}
