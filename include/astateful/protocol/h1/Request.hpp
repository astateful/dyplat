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

#include "astateful/token/h1/Context.hpp"
#include "astateful/token/h1/Machine.hpp"

#include "../Request.hpp"

namespace astateful {
namespace mongo {
  struct Context;
}

namespace protocol {
namespace h1 {
  //!
  //!
  template<typename U, typename V> struct Request : 
    public protocol::Request<U, V> {
    //!
    //!
    Request( const token::h1::Machine& machine,
             const std::wstring& public_html,
             const mongo::Context& mongo,
             const std::wstring& script_path,
             const std::wstring& memoize_path ) :
      protocol::Request<U, V>(),
      m_machine( machine ),
      m_h1( m_machine.initialState() ),
      m_public_html( public_html ),
      m_mongo( mongo ),
      m_script_path( script_path ),
      m_memoize_path( memoize_path ) {}

    //! Given a buffer of unknown size, use the input number of bytes to
    //! examine only the most recent chunk of data in the buffer. Then,
    //! based on the saved FSM state and this chunk, determine if we have
    //! finished reading all the data.
    //!
    //! @param data A buffer whose size may not equal the input length.
    //! @param length The number of bytes that we read from the data.
    //! @return True if the FSM produces a valid transition state.
    //!
    bool update( const uint8_t * data, int length ) override {
      for ( int i = 0; i < length; i++ ) {
        //std::cout << data[i];
        if ( !m_machine( m_h1, m_h1.state, data[i] ) )
          return false;

        if ( m_machine.successState() == m_h1.state )
          return true;
      }

      return false;
    }

    //!
    //!
    V hash() const override;

    //!
    //!
    void clear() override;

    //!
    //!
    std::string upgradeProtocol() override {
      // See if the connection field exists at all first.
      if ( !m_h1.headerAt( "Connection" ) ) return "";

      // Make sure that "Upgrade" header exists somewhere.
      auto connection = m_h1.headerValue( "Connection" );
      if ( connection.find( "Upgrade" ) == std::string::npos ) return "";

      // So we know we have an "Upgrade" connection type,
      // so we need to find the protocol to upgrade to.
      if ( !m_h1.headerAt( "Upgrade" ) ) return "";

      // Right now the only supported upgrade is to websockets.
      if ( m_h1.headerValue( "Upgrade" ) == "websocket" )
        return "ws";

      return "";
    }

    //! Provide a way for a client to choose which executing state
    //! to select based on data provided by the context.
    //!
    bool initAlgorithm( algorithm::Context<U, V>& ) const override;
  private:
    //! The FSM used to evaluate the HTTP request.
    const token::h1::Machine& m_machine;

    //! The machine context used to store parsed HTTP data.
    token::h1::Context m_h1;

    //! The root folder containing web facing public data.
    const std::wstring& m_public_html;

    //! The context used for communicating with the mongo server.
    const mongo::Context& m_mongo;

    //! The root folder where the script files reside.
    const std::wstring& m_script_path;

    //! The path to the memoization storage location (used for scripts).
    const std::wstring& m_memoize_path;
  };
}
}
}
