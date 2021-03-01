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

#include <string>
#include <vector>
#include <cstdint>

namespace astateful {
namespace bson {
  //! Needed for the conversion operators but only ever implemented for
  //! one derived class...an overkill maybe!?
  struct ObjectId;

  //! Define an enumeration for each element which matches the
  //! BSON specification numbers. Each class represents the grammar
  //! for one of those elements, and each class provides the correct
  //! enumeration.
  enum class element_e {
    EOO = 0,
    DOUBLE = 1,
    STRING = 2,
    OBJECT = 3,
    ARRAY = 4,
    BINARY = 5,
    OID = 7,
    BOOL = 8,
    DATETIME = 9,
    EMPTY = 10,
    REGEX = 11,
    CODE = 13,
    CODEWSCOPE = 15,
    INT = 16,
    TIMESTAMP = 17,
    LONG = 18 };

  enum class error_e;

  //! We have defined a separate class for each BSON element. This helps us
  //! create a handy abstraction for the elements when using different platforms.
  //! Each class emits an enumeration type which matches the BSON specification
  //! number.
  struct Element {
    //! Every element must be constructed with at least the key
    //! which describes the element data. In the future we will
    //! support perhaps a wide string key format.
    //!
    explicit Element( const std::string& );
    virtual ~Element() = default;

    //! Delete the custom copy constructor and assignment operator. This
    //! class is generally expensive to construct and the client is
    //! encouraged to move objects instead of copying them to improve
    //! performance.
    //!
    Element( const Element& ) = delete;
    Element& operator=( const Element& ) = delete;

    //! Implement a custom move constructor to handle the member data.
    //!
    Element( Element&& );

    //! Delete the custom move assignment operator since we have a const
    //! member.
    //!
    Element& operator=( Element && ) = delete;

    //! Return the key for this element as a multi-byte string.
    //!
    const std::string& key() const { return m_key; };

    //! Return the key for this element as a wide string.
    //!
    const std::wstring wideKey() const;

    //! Define an insane number of conversion operators so that users are able
    //! to work directly with the underlying type instead of having to do a
    //! static_cast. This isnt the best situation at the moment and perhaps
    //! in the future we could explore a template-based solution.
    //!
    virtual operator element_e() const = 0;
    virtual operator bool() const;
    virtual operator int32_t() const;
    virtual operator int64_t() const;
    virtual operator double() const;
    virtual operator std::vector<uint8_t>() const;
    virtual operator std::wstring() const;
    virtual operator std::string() const;
    virtual operator ObjectId() const;

    //! Return a JSON representation of this element.
    //!
    virtual std::wstring json( bool,
                               const std::wstring&,
                               const std::wstring&,
                               const int ) const = 0;

    //! Return the BSON byte representation of this element.
    //!
    virtual std::vector<uint8_t> bson( error_e& ) const = 0;

    //! Escape a JSON string by preserving all ASCII characters (except
    //! the control characters) in their original format and escaping with
    //! \u all other characters.
    //!
    static std::wstring escapeJSON( const std::wstring& );

    //!
    //!
    static bool validateString( error_e&, const std::string&, bool, bool );

    //!
    //!
    static bool appendInt( std::vector<uint8_t>&, size_t );

    //!
    //!
    static bool appendString( error_e& error, std::vector<uint8_t>& output,
                              const std::string& value );
  private:
    // The key name which describes the element data.
    const std::string m_key;
  };
}
}
