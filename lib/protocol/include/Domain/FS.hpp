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
#include "../State/Node.hpp"
#include "../State/File.hpp"
#include "../State/Path.hpp"

#include "astateful/plugin/Node.hpp"
#include "astateful/plugin/Iterate.hpp"

#include <codecvt>

namespace astateful {
namespace protocol {
  //! The FS domain is instantiated when a file or folder needs to be
  //! processed.
  template<typename U, typename V = std::string> struct DomainFS :
    protocol::Domain < U > {
    //!
    //!
    DomainFS( algorithm::Context<U, V>& algorithm,
              const std::wstring& public_html ) :
      Domain<U, V>( algorithm ),
      m_public_html( public_html ) {}
  protected:
    //!
    const std::wstring& m_public_html;

    //!
    //!
    bool h1( const token::h1::Context& h1 ) override {
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
      std::wstring input( convert.from_bytes( h1.uri ));
      std::wstring file( m_public_html + input );

      plugin::Node node( file );

      m_algorithm.state["node"] = std::make_unique<StateNode<U, V>>( node );

      if ( node.directory() ) {
        m_algorithm.exec = "path";
        m_algorithm.state["path"] = std::make_unique<StatePath<U, V>>();

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "path" ),
                                  std::forward_as_tuple( "path" ) );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "path" ),
                                  std::forward_as_tuple( "method" ) );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "node" ),
                                  std::forward_as_tuple( "path",
                                  std::vector<std::string>{ "method" } ) );
      } else {
        m_algorithm.exec = "file";
        m_algorithm.state["file"] = std::make_unique<StateFile<U, V>>();

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "file" ),
                                  std::forward_as_tuple( "file" ) );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "file" ),
                                  std::forward_as_tuple( "method" ) );

        m_algorithm.flux.emplace( std::piecewise_construct,
                                  std::forward_as_tuple( "node" ),
                                  std::forward_as_tuple( "file",
                                  std::vector<std::string>{ "method" } ) );
      }

      return true;
    }
  };
}
}