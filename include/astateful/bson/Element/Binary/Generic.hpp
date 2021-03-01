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
  //! Define an element class representing a BSON binary type 'generic'. As per
  //! the BSON specification, this is the most commonly used binary subtype and
  //! should be the default for drivers and tools.
  struct ElementBinaryGeneric : public ElementBinary {
    //! Construct a BSON element class representing the given data.
    //!
    ElementBinaryGeneric( const std::string&, const std::vector<uint8_t>& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementBinaryGeneric( const std::string&, const std::string& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementBinaryGeneric( const std::string&, const uint8_t *, const size_t );

    //! Construct a BSON element class representing the given data.
    //!
    ElementBinaryGeneric( const std::string&, std::vector<uint8_t>::const_iterator,
                          std::vector<uint8_t>::const_iterator );

    //! Implement a custom move constructor to handle moving member data.
    //!
    ElementBinaryGeneric( ElementBinaryGeneric&& );

    //! Return the BSON specification enumeration for this element.
    //!
    operator sub_e() const override { return sub_e::GENERIC; }

    //! Return a JSON representation of this element.
    //!
    std::wstring json( bool,
                       const std::wstring&,
                       const std::wstring&,
                       const int ) const override;
  };
}
}
