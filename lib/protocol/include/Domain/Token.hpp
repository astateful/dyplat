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

#include "../Domain.hpp"
#include "../State/Token.hpp"
#include "../State/Body.hpp"

namespace astateful {
namespace protocol {
  //! The token domain is used to obtain, invalidate, and refresh auth tokens.
  template<typename U, typename V = std::string> struct DomainToken :
    protocol::Domain<U, V> {
    //!
    //!
    DomainToken( algorithm::Context<U, V>& algorithm,
                 const mongo::Context& mongo ) :
      Domain<U, V>( algorithm ),
      m_mongo( mongo ) {}
  protected:
    //!
    const mongo::Context& m_mongo;

    //!
    //!
    bool h1( const token::h1::Context& h1 ) override {
      if ( h1.uri == "/obtain" ) {
        m_algorithm.exec = "token";
        m_algorithm.state["token"] = std::make_unique<StateToken<U, V>>( m_mongo, "obtain" );
        m_algorithm.state["body"] = std::make_unique<StateBody<U, V>>( h1.body );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "body" ),
                                  std::forward_as_tuple( "body" ) );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "token" ),
                                  std::forward_as_tuple( "method" ) );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "token" ),
                                  std::forward_as_tuple( "body" ) );
        return true;
      }

      return false;
    }
  };
}
}