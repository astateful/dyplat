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
#include "../State/Connection.hpp"
#include "../State/Document.hpp"
#include "../State/Graph.hpp"
#include "../State/Auth.hpp"
#include "../State/Body.hpp"

#include "astateful/token/graph/Context.hpp"
#include "astateful/token/graph/Machine.hpp"

namespace astateful {
namespace protocol {
  //! The graph domain acts as an API for mongodb.
  template<typename U, typename V = std::string> struct DomainGraph :
    protocol::Domain<U, V> {
    //!
    //!
    DomainGraph( algorithm::Context<U, V>& algorithm,
                 const mongo::Context& mongo ) :
      Domain<U, V>( algorithm ),
      m_mongo( mongo) {}
  protected:
    //!
    const mongo::Context& m_mongo;

    //!
    //!
    bool h1( const token::h1::Context& h1 ) override {
      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "body" ),
                                std::forward_as_tuple( "body" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "auth" ),
                                std::forward_as_tuple( "auth",
                                std::vector < std::string > {"query"} ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "document" ),
                                std::forward_as_tuple( "id" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "document" ),
                                std::forward_as_tuple( "document",
                                std::vector < std::string > {"connection", "query", "auth", "body", "method"} ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "connection" ),
                                std::forward_as_tuple( "connection",
                                std::vector < std::string > {"body", "auth", "query", "id", "method"} ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "graph" ),
                                std::forward_as_tuple( "document" ) );

      m_algorithm.flux.emplace( std::piecewise_construct,
                                std::forward_as_tuple( "graph" ),
                                std::forward_as_tuple( "method" ) );

      // Initiate the FSM to try to parse the URI into a graph request.
      const token::graph::Machine machine;
      token::graph::Context context( machine.initialState() );

      for ( const auto& c : h1.uri ) {
        if ( !machine( context, context.state, c ) ) {
          return false;
        }
      }

      if ( context.state != machine.successState() ) return false;

      // There is always a document present in a graph query.
      m_algorithm.state["document"] = std::make_unique<StateDocument<U, V>>(
        m_mongo,
        context.document );

      // However the connection is optional.
      if ( context.connection.size() > 0 )
        m_algorithm.state["connection"] = std::make_unique<StateConnection<U, V>>(
          m_mongo,
          context.connection );

      m_algorithm.state["graph"] = std::make_unique<StateGraph<U, V>>();
      m_algorithm.state["auth"] = std::make_unique<StateAuth<U, V>>( m_mongo );
      m_algorithm.state["body"] = std::make_unique<StateBody<U, V>>( h1.body );
      m_algorithm.exec = "graph";

      return true;
    }
  };
}
}