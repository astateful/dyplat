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
#include "astateful/plugin/Node.hpp"

namespace astateful {
namespace protocol {
  //! The node state handles a file or directory node in the file system.
  template<typename U, typename V> struct StateNode : public algorithm::State<U, V> {
    //! constructor
    //!
    //! @param active Determines if this state participates in the evaluation.
    //! @param data The internal data which is outputted by this state.
    //!
    StateNode( const plugin::Node& data ) :
      m_data( data ),
      algorithm::State<U, V>() {}

    //! Return a new unknown based on the flux index as well as the values
    //! provided by the given value map.
    //!
    std::unique_ptr<U> operator()( const algorithm::value_t<U, V>&,
                                   const V& ) const override;
  private:
    //! The internal data which is outputted by this state.
    const plugin::Node m_data;
  };
}
}
