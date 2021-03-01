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
#include "astateful/bson/ObjectId.hpp"

namespace astateful {
namespace bson {
  //! Define an element class representing a BSON ObjectId. The class allows us
  //! to output the value in a variety of formats. Per the BSON specification,
  //! 'An ObjectId is a 12-byte value consisting of a 4-byte timestamp which are
  //! the seconds since epoch), a 3-byte machine id, a 2-byte process id, and a
  //! 3-byte counter. Note that the timestamp and counter fields must be stored
  //! big endian unlike the rest of BSON. This is because they are compared
  //! byte-by-byte and we want to ensure a mostly increasing order.'
  struct ElementObjectId final : public Element {
    //! Construct a BSON element class using generated data.
    //!
    ElementObjectId( const std::string& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementObjectId( const std::string&, const std::string& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementObjectId( const std::string&, const std::vector<uint8_t>& );

    //! Construct a BSON element class representing the given data.
    //!
    ElementObjectId( const std::string&, const ObjectId& );

    //! Implement a custom move constructor to handle moving member data.
    //!
    ElementObjectId( ElementObjectId&& );

    //! Return the BSON specification enumeration for this element.
    //!
    operator element_e() const override { return element_e::OID; }

    //! Return a byte representation of the internal data.
    //!
    operator std::vector<uint8_t>() const override { return m_data; };

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

    //! Return a ObjectId representation of the internal data.
    //!
    operator ObjectId() const override { return m_data; }

    //! Return the BSON byte representation of this element.
    //!
    std::vector<uint8_t> bson( error_e& ) const override { return m_data; };
  private:
    //! Represent the ObjectId simply as itself.
    const ObjectId m_data;
  };
}
}
