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
namespace protocol {
  //! The method state is needed to tell the rest of the states how to operate on
  //! the input. Was the intent of this evaluation instance to show data, to
  //! delete data, to update data...?
  template<typename U, typename V> struct StateMethod : public algorithm::State<U, V> {
    //! constructor
    //!
    //! @param data The internal data which is outputted by this state.
    //!
    StateMethod( const std::string& data ) :
      algorithm::State<U, V>(),
      m_data( data )
    {}

    //! Return a new unknown based on the flux index as well as the values
    //! provided by the given value map.
    //!
    std::unique_ptr<U> operator()( const algorithm::value_t<U, V>&, const V& ) const override;
  private:
    //! The internal data which is outputted by this state.
    const std::string& m_data;
  };
}
}
