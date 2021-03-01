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

#include <unordered_map>

namespace astateful {
namespace algorithm {
  //! A flux describes how a value is transferred between states. A state
  //! which emits a flux that has no dependencies can create its value right
  //! away, whereas a flux that has dependencies needs values to be created
  //! first from those dependencies before they can be pushed.
  template<typename V> struct Flux final {
    //! A default flux can never be constructed.
    //!
    Flux() = delete;

    //! Construct a flux without any dependencies. Technically at this point
    //! the flux could either be pushing or pulling. If the flux is assigned to
    //! the executing state then it is pulling, else it is pushing.
    //!
    //! @param index The index value of this flux.
    //!
    Flux( V index ) : m_index( index ), m_dependency() {}

    //! Construct a flux with a list of flux dependencies that need to be
    //! satisfied before a value can be created.
    //!
    //! @param index The index value of this flux.
    //! @param dependency The dependencies that this flux needs satisfied.
    //!
    Flux( V index, const std::vector<V>& dependency ) :
      m_index( index ),
      m_dependency( dependency ) {}

    //! A flux never needs to be copied or moved since it functions as an
    //! independent entity and as such has no repeatable attributes that
    //! the programmer could exploit in building a new flux.
    //!
    Flux( const Flux& ) = delete;
    Flux& operator=( const Flux& ) = delete;
    Flux& operator=( Flux&& ) = delete;
    Flux( Flux&& ) = delete;

    //! Return the dependencies that this flux needs satisfied.
    //!
    const std::vector<V>& dependency() const { return m_dependency; }

    //! A convenience method to use the fluxes index and the class
    //! interchangeably.
    //!
    //! @return The flux index which corresponds to a value index.
    //!
    operator const V& () const { return m_index; }
  private:
    //! The index value of this flux.
    const V m_index;

    //! The dependencies that this flux needs satisfied.
    const std::vector<V> m_dependency;
  };

  //! Define a managed type for our flux container.
  template<typename V> using flux_t = std::unordered_multimap<V, const Flux<V>>;
}
}
