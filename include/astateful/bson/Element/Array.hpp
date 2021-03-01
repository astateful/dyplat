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
  //! Forward declare since the data is actually a serialization.
  struct Serialize;

  //! Define an element class representing a BSON array value. The class allows
  //! us to output the value in a variety of formats. Per the BSON specification,
  //! the document for an array is a normal BSON document with integer values for
  //! the keys, starting with 0 and continuing sequentially. For example, the
  //! array ['red', 'blue'] would be encoded as the document
  //! {'0': 'red', '1': 'blue'}. The keys must be in ascending numerical order.
  struct ElementArray final : public Element {
    //! Construct a BSON element class representing the given data.
    //!
    ElementArray( const std::string& key,
                  const std::vector<uint8_t>& data );

    //! Construct a BSON element class representing the given data.
    //!
    ElementArray( const std::string& key,
                  const Serialize& data,
                  error_e& error );

    //! Implement a custom move constructor to handle moving member data.
    //!
    ElementArray( ElementArray&& );

    //! Return the BSON specification enumeration for this element.
    //!
    operator element_e() const override { return element_e::ARRAY; }

    //! Return a byte representation of the internal data.
    //!
    operator std::vector<uint8_t>() const override { return m_data; }

    //! Return a wide string representation of the internal data.
    //!
    operator std::wstring() const override;

    //! Return a multi-byte string representation of the internal data.
    //!
    operator std::string() const override;

    //! Return the BSON byte representation of this element.
    //!
    std::vector<uint8_t> bson( error_e& ) const override { return m_data; }

    //! Return a JSON representation of this element.
    //!
    std::wstring json( bool,
                       const std::wstring&,
                       const std::wstring&,
                       const int ) const override;
  private:
    //! Represent the copied binary data as a managed byte vector.
    std::vector<uint8_t> m_data;
  };
}
}
