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

#include "h1/Transform.hpp"
#include "h1/Request.hpp"
#include "h1/Response.hpp"
#include "h2/Transform.hpp"
#include "h2/Request.hpp"
#include "h2/Response.hpp"
#include "ws/Transform.hpp"
#include "ws/Request.hpp"
#include "ws/Response.hpp"

namespace astateful {
namespace mongo {
  //!
  struct Context;
}

namespace protocol {
  //!
  enum class state_e {
    upgrade,
    send,
    recv
  };

  //!
  template<typename U, typename V> struct Context {
    //!
    //!
    Context( const std::wstring& memoize_path,
             const std::wstring& public_html,
             const std::wstring& script_path,
             const mongo::Context& mongo,
             const algorithm::Parse<U, V>& parse,
             const algorithm::Generate<U, V>& generate,
             const plugin::Pipe<U>& pipe ) :
             m_memoize_path( memoize_path ),
             m_public_html( public_html ),
             m_script_path( script_path ),
             m_mongo( mongo ),
             m_compress(),
             m_expand(),
             m_parse( parse ),
             m_generate( generate ),
             m_pipe( pipe ) {}

    //!
    //!
    state_e initialState( const std::string& protocol ) const {
      if ( protocol == "h1" || protocol == "http/1.1" )
        return state_e::upgrade;

      //if ( protocol == "h2" || protocol == "h2-15" || protocol == "h2-14" )
      //  return state_e::send;

      return state_e::recv;
    }

    /*state_e transition( state_e input ) {
      switch (input) {
        case state_e::init: {
          return state_e::recv;
        }
        break;
      }

      return input;
    }*/

    //!
    //!
    Request<U, V> * generateRequest( const std::string& protocol ) const {
      if ( protocol == "h1" || protocol == "http/1.1" )
        return new h1::Request<U, V>(
          h1_machine,
          m_public_html,
          m_mongo,
          m_script_path,
          m_memoize_path );

      if ( protocol == "h2" || protocol == "h2-15" || protocol == "h2-14" )
        return new h2::Request<U, V>(
          h2_machine,
          m_public_html,
          m_mongo,
          m_script_path,
          m_memoize_path );

      if ( protocol == "ws" )
        return new ws::Request<U, V>( ws_machine );

      return nullptr;
    }

    //!
    Response<U, V> * generateResponse( const std::string& protocol ) const {
      if ( protocol == "h1" || protocol == "http/1.1" ) {
        return new h1::Response<U, V>( m_pipe,
                                       m_compress,
                                       m_expand,
                                       m_generate,
                                       m_parse,
                                       h1_transform );
      } else if ( protocol == "h2" || protocol == "h2-15" || protocol == "h2-14" ) {
        return new h2::Response<U, V>( m_pipe,
                                       m_compress,
                                       m_expand,
                                       m_generate,
                                       m_parse,
                                       h2_transform );
      } else if ( protocol == "ws" ) {
        return new ws::Response<U, V>( m_pipe,
                                       m_compress,
                                       m_expand,
                                       m_generate,
                                       m_parse,
                                       ws_transform );
      }

      return nullptr;
    }
  private:
    //! The FSM used to evaluate the Websocket request.
    const byte::ws::Machine ws_machine;

    //! The FSM used to evaluate the HTTP request.
    const token::h1::Machine h1_machine;

    //!
    const byte::h2::Machine h2_machine;

    //!
    const h1::Transform<U, V> h1_transform;
    
    //!
    const ws::Transform<U, V> ws_transform;

    //!
    const h2::Transform<U, V> h2_transform;

    //! The path to the memoization storage location (used for scripts).
    const std::wstring& m_memoize_path;

    //! The root folder containing web facing public data.
    const std::wstring& m_public_html;

    //! The root folder where the script files reside.
    const std::wstring& m_script_path;

    //!
    const mongo::Context& m_mongo;

    //!
    const plugin::Compress<U, V> m_compress;

    //!
    const plugin::Expand<U, V> m_expand;

    //!
    const algorithm::Parse<U, V>& m_parse;

    //!
    const algorithm::Generate<U, V>& m_generate;

    //!
    const plugin::Pipe<U>& m_pipe;
  };
}
}
