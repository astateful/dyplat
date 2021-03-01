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

#include <astateful/token/h1/Context.hpp>
#include <astateful/algorithm/Context.hpp>

#include "State/Query.hpp"
#include "State/Method.hpp"
#include "State/Header.hpp"
#include "State/Version.hpp"

namespace astateful {
namespace protocol {
  //! A domain is instantiated from the host value read in from a request.
  //! It is used to create an algorithmic context and thus setup an evaluation.
  template<typename U, typename V = std::string> struct Domain {
    //!
    //!
    Domain( algorithm::Context<U, V>& algorithm ) :
      m_algorithm( algorithm ) {}

    virtual ~Domain() {}

    //!
    //!
    bool operator()( const token::h1::Context& h1 ) {
      m_algorithm.state["query"] = std::make_unique<StateQuery<U, V>>( h1 );
      m_algorithm.state["method"] = std::make_unique<StateMethod<U, V>>( h1.method );
      m_algorithm.state["header"] = std::make_unique<StateHeader<U, V>>( h1 );
      m_algorithm.state["version"] = std::make_unique<StateVersion<U, V>>( h1.version );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "query" ),
                                std::forward_as_tuple( "query" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "header" ),
                                std::forward_as_tuple( "header" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "method" ),
                                std::forward_as_tuple( "method" ) );

      return this->h1( h1 );
    }
  protected:
    //!
    //!
    algorithm::Context<U, V>& m_algorithm;

    //!
    //!
    virtual bool h1( const token::h1::Context& ) = 0;
  };
}
}
