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

#include "astateful/bson/Element/Binary.hpp"

namespace astateful {
namespace bson {
  //! Define an element class representing a BSON binary type 'MD5'.
  struct ElementBinaryMD5 : public ElementBinary {
    //! Construct a BSON element class representing the given data.
    //!
    ElementBinaryMD5( const std::string&, const std::vector<uint8_t>& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementBinaryMD5( const std::string&, const std::string& );

    //! Implement a custom move constructor to handle moving member data.
    //!
    ElementBinaryMD5( ElementBinaryMD5&& );

    //! Return the BSON specification enumeration for this element.
    //!
    operator sub_e() const override { return sub_e::MD5; }

    //! Return a JSON representation of this element.
    //!
    std::wstring json( bool,
                       const std::wstring&,
                       const std::wstring&,
                       const int ) const override;
  };
}
}
