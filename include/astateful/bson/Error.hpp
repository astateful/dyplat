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

#include <type_traits>

namespace astateful {
namespace bson {
  //! Status codes returned from interacting with the serialize.
  enum class error_e : int {
    CLEAN = 0,
    NOT_UTF8 = ( 1 << 1 ),
    FIELD_HAS_DOT = ( 1 << 2 ),
    FIELD_INIT_DOLLAR = ( 1 << 3 ),
    TOO_LARGE = ( 1 << 4 )
  };

  using error_t = std::underlying_type_t<error_e>;

  inline error_e operator|( error_e lhs, error_e rhs ) {
    return (error_e)( static_cast<error_t>( lhs ) |
                    static_cast<error_t>( rhs ) );
  }

  inline error_e& operator|=( error_e& lhs, error_e rhs ) {
    lhs = (error_e)( static_cast<error_t>( lhs ) |
                   static_cast<error_t>( rhs ) );
    return lhs;
  }

  inline error_e operator&( error_e lhs, error_e rhs ) {
    return (error_e)( static_cast<error_t>( lhs ) &
                    static_cast<error_t>( rhs ) );
  }

  inline error_e operator~( error_e error ) {
    return (error_e)~static_cast<error_t>( error );
  }

  inline error_e& operator&=( error_e& lhs, error_e rhs ) {
    return (error_e&)( reinterpret_cast<error_t&>( lhs ) &=
                     static_cast<error_t>( rhs ) );
  }
}
}
