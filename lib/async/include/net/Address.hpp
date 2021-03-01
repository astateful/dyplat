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

#include <memory>
#include <unordered_map>
#include <string>

#include "Ws2tcpip.h"

namespace astateful {
namespace token {
namespace address {
  struct Context;
}
}

namespace async {
namespace net {
  //! Provide an abstraction of the various Windows flavors of address
  //! structures. By address we mean a port and an IP. Note that this class
  //! is only really useful for debugging; all of the important Winsock info
  //! is lost since all relevant information is copied to strings.
  struct Address final {
    //! Make sure to never create an invalid address object.
    //!
    Address() = delete;

    //!
    //!
    explicit Address( const SOCKADDR_STORAGE& );

    //!
    //!
    explicit Address( const ADDRINFOW * );

    //! Construct an address using a tokenized context. The context is assumed
    //! to be generated from the corresponding address machine and is thus
    //! assumed to be valid.
    //!
    //! @param context A valid tokenized context to create an address with.
    //!
    Address( const token::address::Context& context );

    //!
    //!
    Address( const Address& );

    //!
    //!
    Address( Address&& );

    //!
    //!
    Address& operator=( const Address& );

    //!
    //!
    Address& operator=( Address && );

    //!
    //!
    operator std::string() const;

    //!
    //!
    bool info( const ADDRINFOW&, PADDRINFOW& ) const;
  private:
    //!
    std::wstring m_port;

    //!
    std::wstring m_hostname;
  };

  //! Implement output stream support for an address. This is only really
  //! used for logging.
  //!
  std::ostream& operator<<( std::ostream&, const Address& );

  //! Parse an input string into an address. If the input string is invalid
  //! then an empty ptr is returned. Note that the only way to truly validate
  //! an address is to use it to connect to the address. This method basically
  //! only ever validates that the input is valid.
  //!
  //! @param input The input address of the form host::port.
  //! @return A non empty address value if successful.
  //!
  std::unique_ptr<Address> generate_address( const std::wstring& input );
}
}
}
