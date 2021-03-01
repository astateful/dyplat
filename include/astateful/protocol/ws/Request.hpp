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

#include "astateful/byte/ws/Context.hpp"
#include "astateful/byte/ws/Machine.hpp"

#include "../Request.hpp"

namespace astateful {
namespace protocol {
namespace ws {
  //!
  //!
  template<typename U, typename V> struct Request : 
    public protocol::Request<U, V> {
    //!
    //!
    Request( const byte::ws::Machine& machine  ) :
      protocol::Request<U, V>(),
      m_machine( machine ),
      m_ws( m_machine.initialState() ) {}

    bool update( const uint8_t * data, int length ) override {
      for ( int i = 0; i < length; i++ ) {
        //std::cout << data[i];
        if ( !m_machine( m_ws, m_ws.state, data[i] ) )
          return false;

        if ( m_machine.successState() == m_ws.state )
          return true;
      }

      return false;
    }

    //!
    //!
    void clear() override;

    //!
    //!
    V hash() const override;

    //! Provide a way for a client to choose which executing state
    //! to select based on data provided by the context.
    //!
    bool initAlgorithm( algorithm::Context<U, V>& ) const override;
  private:
    //! The FSM used to evaluate the Websocket request.
    const byte::ws::Machine& m_machine;

    //! The machine context used to store parsed Websocket data.
    byte::ws::Context m_ws;
  };
}
}
}