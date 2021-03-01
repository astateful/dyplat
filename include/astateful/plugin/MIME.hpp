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
#include <unordered_map>

namespace astateful {
namespace plugin {
  //! Used to map a MIME type to an enumeration.
  enum class mime_e {
    UNKNOWN,
    HTML,
    HTM,
    CSS,
    JS,
    ICO,
    GIF,
    JPG,
    JPEG,
    PNG,
    TXT,
    XML,
    JSON,
    PDF,
    BMP
  };

  //! A MIME type maps a file extension to a type that is valid for usage on the
  //! internet (web browsers, email, etc). The MIME type (its extension as well
  //! as its type) is grouped under an enumeration so that it is easily storable
  //! within a serialization. There is only the penalty of mapping the extension
  //! to the enumeration (reverse lookup) when a file is found.
  struct MIME final {
    //! constructor
    //!
    MIME( mime_e, const std::string& );

    //!
    //!
    MIME( const MIME& ) = delete;
    MIME& operator=( const MIME& ) = delete;

    //!
    //!
    operator mime_e() const;

    //!
    //!
    const std::string& type() const;
  private:
    //!
    const mime_e m_mime;

    //!
    const std::string m_type;
  };

  //! Define a managed type for our MIME container.
  using mime_t = std::unordered_map<std::wstring, MIME>;
}
}
