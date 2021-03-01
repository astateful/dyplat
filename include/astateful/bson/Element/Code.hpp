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

namespace astateful {
namespace bson {
  //! Define an element class representing a BSON code value. The class allows us
  //! to output the value in a variety of formats. Per the BSON specification,
  //! the code value is a null terminated string encoded in UTF-8.
  struct ElementCode final : public Element {
    //! Construct a BSON element class representing the given data.
    //!
    ElementCode( const std::string&, const std::wstring& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementCode( const std::string&, const std::vector<uint8_t>& );

    //! Implement a custom move constructor to handle moving member data.
    //!
    ElementCode( ElementCode&& );

    //! Return the BSON specification enumeration for this element.
    //!
    operator element_e() const override { return element_e::CODE; }

    //! Return a wide string representation of the internal data.
    //!
    operator std::wstring() const override;

    //! Return a multi-byte string representation of the internal data.
    //!
    operator std::string() const override { return m_data; }

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
    //! Represent the code data internally as a string.
    std::string m_data;
  };
}
}
