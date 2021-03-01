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

#include <cstdint>
#include <iterator>
#include <memory>
#include <vector>

namespace astateful {
namespace bson {
  struct Element;

  struct Serialize;

  enum class error_e;

  enum class element_e;

  //! The BSON iterator is specifically designed to iterate over data that is
  //! formatted according to the BSON specification. It takes the current element
  //! type and generates a class around the data, then pushes that class to a
  //! location where it can be saved for the caller after the increment is complete.
  struct Iterate : public std::iterator<std::input_iterator_tag, Element> {
    //! Create a new iterate from byte data which should be in the BSON
    //! format. The constructor stores the first key and element, but we do
    //! NOT increment the iterator.
    //!
    Iterate( const std::vector<uint8_t>& );

    //!
    //!
    Iterate( const Serialize&, error_e& );

    //! Delete the custom copy constructor and assignment operator. This class is
    //! generally expensive to construct and the client is encouraged to move
    //! objects instead of copying them to improve performance.
    //!
    Iterate( const Iterate& ) = delete;

    //! Delete the custom copy assignment operator. This class is generally
    //! expensive to construct and the client is encouraged to move objects
    //! instead of copying them to improve performance.
    //!
    Iterate& operator=( const Iterate& ) = delete;

    //! pre increment operator
    //!
    Iterate& operator++();

    //! Define a boolean operator which is used to test the validity of
    //! the iterator during a looping process.
    //!
    operator bool() const;

    //! Implement the dereference operator for the element that the
    //! iteration currently points to.
    //!
    Element& operator*();

    //! Implement the member access operator for the element that the
    //! iteration currently points to.
    //!
    Element * operator->();

    //!
    //!
    element_e enumId() const;

    //! Return the current element. This is useful for clients who wish
    //! to "steal" elements without copying them. This can be a very dangerous
    //! method to use but it is required.
    //!
    std::unique_ptr<Element>& current();
  private:
    //! The states used to parse the input binary data.
    enum class state_e {
      TYPE,
      NAME,
      NAME_SIZE,
      ELEMENT,
      SUBTYPE,
      STRING,
      REGEX,
      PATTERN,
      OPTION,
      TIMESTAMP,
      SIZE,
      CODE_SIZE,
      END
    };

    //! Used to store the last element that was created by the iterator.
    //! Further increment operations will override this memory with
    //! a new element.
    std::unique_ptr<Element> m_element;

    //! Stores our current position within the copied binary data.
    size_t m_position;

    //! Store a reference to the binary data to be iterated over. This
    //! results in a tremendous performance gain for the iterate.
    std::vector<uint8_t> m_data;

    //! A common buffer used to store the currently parsed element. By
    //! common we mean that many methods are using this buffer (but
    //! sequentially of course).
    std::vector<uint8_t> m_buffer;

    //! The current enumeration value of the iterator as it parses.
    state_e m_state;

    //! Push an element onto the stack. We call this method twice: once
    //! within the constructor to initialize the initial pair, as well
    //! after iterating, when a new pair is expected to exist.
    //!
    void PushElement( uint8_t );

    //! Return a new BSON array element.
    //!
    Element * GenerateElementArray();

    //! Return a new BSON binary element.
    //!
    Element * GenerateElementBinary();

    //! Return a new BSON boolean element.
    //!
    Element * GenerateElementBool();

    //! Return a new BSON code element.
    //!
    Element * GenerateElementCode();

    //! Return a new BSON code with scope element.
    //!
    Element * GenerateElementCodescope();

    //! Return a new BSON datetime element.
    //!
    Element * GenerateElementDatetime();

    //! Return a new BSON double element.
    //!
    Element * GenerateElementDouble();

    //! Return a new BSON int element.
    //!
    Element * GenerateElementInt();

    //! Return a new BSON long element.
    //!
    Element * GenerateElementLong();

    //! Return a new BSON null element.
    //!
    Element * GenerateElementNull();

    //! Return a new BSON object element.
    //!
    Element * GenerateElementObject();

    //! Return a new BSON object id element.
    //!
    Element * GenerateElementObjectId();

    //! Return a new BSON regex element.
    //!
    Element * GenerateElementRegex();

    //! Return a new BSON string element.
    //!
    Element * GenerateElementString();

    //! Return a new BSON timestamp element.
    //!
    Element * GenerateElementTimestamp();

    //! Convert the current buffer contents into a string.
    //!
    void ToString( std::string& ) const;

    //! Convert the current buffer contents into an integer.
    //!
    void ToInt( int& ) const;

    //! Convert the current buffer contents into a long.
    //!
    void ToLong( int64_t& ) const;

    //! Convert the current buffer contents into a double.
    //!
    void ToDouble( double& ) const;

    //! In order that the serialization class can append an iterator,
    //! it needs to access the private iterator data directly.
    friend struct Serialize;
  };
}
}
