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

#include "astateful/bson/Element.hpp"

struct _FILETIME;

namespace astateful {
namespace bson {
  //! Define an element class representing a BSON datetime value. The class
  //! allows us to output the value in a variety of formats. Per the BSON
  //! specification, the datetime value is 8 bytes (64-bit signed integer).
  struct ElementDatetime : public Element {
    //! Construct a BSON element class representing the given data.
    //!
    ElementDatetime( const std::string&, int64_t );

    //! Construct a BSON element class representing the given data.
    //!
    ElementDatetime( const std::string& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementDatetime( const std::string&, const _FILETIME& );

    //! Implement a custom move constructor to handle moving member data.
    //!
    ElementDatetime( ElementDatetime&& );

    //! Return the BSON specification enumeration for this element.
    //!
    operator element_e() const override { return element_e::DATETIME; }

    //! Return a long representation of the internal data.
    //!
    operator int64_t() const override { return m_data; };

    //! Return a byte representation of the internal data.
    //!
    operator std::vector<uint8_t>() const override;

    //! Return a wide string representation of the internal data.
    //!
    operator std::wstring() const override;

    //! Return a multi-byte string representation of the internal data.
    //!
    operator std::string() const override;

    //! Return a JSON representation of this element.
    //!
    std::wstring json( bool,
                       const std::wstring&,
                       const std::wstring&,
                       const int ) const override;

    //! Return the BSON byte representation of this element.
    //!
    std::vector<uint8_t> bson( error_e& ) const override;
  private:
    //! The internal data in the correct BSON storage format.
    int64_t m_data;
  };
}
}
