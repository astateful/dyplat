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

#include <vector>
#include <cstdint>

namespace astateful {
namespace protocol {
  //! Provide an interface for transforming a value computed from the algorithm
  //! into a byte response.
  template<typename U, typename V> struct Transform {
    //! Default construct this class.
    //!
    Transform() = default;
    virtual ~Transform() = default;

    //! In order for a transformation to take place, the input data is required
    //! along with the id of the executing state and the output buffer which is
    //! the transformed input. The executing state is required sometimes since
    //! the transformation might need to know something extra about the input
    //! data.
    //!
    //! @param input The input value is the final algorithmic result.
    //! @param exec Is the key of the executing state to aid in transforming.
    //! @param output The output vector to populate.
    //! @return True if the transformation was successful, false otherwise.
    //!
    virtual bool operator()( const U& input,
                             const V& exec,
                             std::vector<uint8_t>& output ) const = 0;
  };
}
}
