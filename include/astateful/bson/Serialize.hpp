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

#include <sstream>
#include <map>
#include <memory>
#include <vector>

namespace astateful {
namespace bson {
  //!
  struct Element;

  enum class error_e;

  //! The BSON serialize class creates a binary representation of structured data
  //! using the BSON specification to provide the formatting rules.
  struct Serialize {
    //! Default construct this class.
    //!
    Serialize() = default;

    //! Initialize this serialization object with data that has already
    //! been serialized, such as data from a file.
    //!
    explicit Serialize( const std::vector<uint8_t>& );

    //! Delete the custom copy constructor and assignment operator. This
    //! class is generally expensive to construct and the client is
    //! encouraged to move objects instead of copying them to improve
    //! performance.
    //!
    Serialize( const Serialize& ) = delete;
    Serialize& operator=( const Serialize& ) = delete;

    //! Add a custom move constructor to take advantage of move semantics.
    //!
    Serialize( Serialize&& );

    //! Add a custom move assignment operator to take advantage of move
    //! semantics.
    //!
    Serialize& operator=( Serialize && );

    //! Append a BSON element to our serialization. Rather than have to
    //! define many overloads to account for all possible element types,
    //! a templated method is defined.
    //!
    //! @param element The element to add to this serialization.
    //!
    template<typename T> void append( T&& element ) {
      m_element.emplace_back( new T( std::forward<T>( element ) ) );
    }

    //! Append a serialization to the serialization under a given key.
    //! This will append a serialization of type object.
    //!
    bool append( const std::string&, const Serialize&, error_e& );

    //! Append a serialization to the serialization under a given key.
    //! This will append a serialization of type array.
    //!
    bool appendArray( const std::string&, const Serialize&, error_e& );

    //! Append an ordered map of serializations to the serialization.
    //! The map will be appended as an array.
    //!
    bool append( const std::string&, const std::map<int, Serialize>&,
                 error_e& );

    //! Provide the ability to output this BSON serialization to a stream.
    //! Note that since this is a friend method it is not a part of the
    //! class and therefore must be implemented elsewhere.
    //!
    friend std::wostream& operator<<( std::wostream&, const Serialize& );

    //! Output the BSON byte representation of this serialization. If this
    //! method fails then the client is required to check the value of inputted
    //! error argument to decide what to do.
    //!
    bool bson( std::vector<uint8_t>&, error_e& ) const;

    //! Print out this serialization as a nicely formatted JSON string.
    //!
    std::wstring json( int, bool ) const;

    //!
    //!
    Element& operator[]( const char * );

    //!
    //!
    Element * at( const std::string& ) const;

    //! A destructor is required due to forward declarations.
    //!
    ~Serialize();
  private:
    //!
    std::vector<std::unique_ptr<Element>> m_element;
  };

  //! Implement the friend method declared in the BSON serialization.
  //!
  std::wostream& operator<<( std::wostream&, const Serialize& );
}
}
