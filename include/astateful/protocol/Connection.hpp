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

#include "astateful/async/net/server/Connection.hpp"

#include "Context.hpp"

namespace astateful {
namespace protocol {
  //!
  template<typename U, typename V> struct Connection final :
    public async::net::server::Connection {
    //!
    //!
    Connection( const Context<U, V>& context,
                const async::net::Address& local,
                const async::net::Address& remote,
                const std::string protocol ) :
                async::net::server::Connection(
                local,
                remote ),
                m_context( context ),
                m_response( context.generateResponse( protocol ) ),
                m_request( context.generateRequest( protocol ) ),
                m_state( context.initialState( protocol ) ) {}

    virtual ~Connection() = default;
  protected:
    //!
    state_e m_state;

    //!
    const Context<U, V>& m_context;

    //! The object used to provide a response for this connection.
    std::unique_ptr<Response<U, V>> m_response;

    //!
    std::unique_ptr<Request<U, V>> m_request;

    //!
    //!
    void respond( std::vector<uint8_t>& output,
                  const async::pipe::client::Engine& pipe ) {
      // Always ensure that the buffer is filled.
      if ( !m_response->create( *m_request, pipe, output ) )
        m_response->fail( output );

      if ( m_state == state_e::upgrade ) {
        // See if we can upgrade the response and the request...
        auto upgrade = m_request->upgradeProtocol();
        if ( upgrade != "" ) {
          // Retrieve the executing state before response is reset.
          const auto exec = m_response->exec();

          // Reset the request to the new protocol.
          m_request.reset( m_context.generateRequest( upgrade ) );

          // Upgrade the response and set the executing state to what it
          // was before the reset; this maintains the transition between
          // protocols.
          m_response.reset( m_context.generateResponse( upgrade ) );
          m_response->setExec( exec );
        }
      }
    }

    //!
    //!
    bool recvEvent( DWORD transferred,
                    const async::pipe::client::Engine& pipe ) override {
      if ( m_request->update( m_wsa_buf.buf, transferred ) ) {
        //!
        respond( m_buffer, pipe );

        m_request->clear();
        m_response->clear();

        m_flag = flag_e::SEND;
        m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
        m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
      }

      return true;
    }

    //!
    //!
    bool sendEvent( DWORD transferred,
                    const async::pipe::client::Engine& pipe ) override {
      // The length is incremented on every send by the amount that was
      // transferred. So the original length should always be the size of
      // the entire response for the increment to work.
      if ( m_wsa_buf.len - transferred == 0 ) {

        m_buffer.clear();

        //ss = m_response->transition( ss );
        /*if ( ss == state_e::send ) {
          auto res = m_response->create( m_buffer );

          m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
          m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
          m_flag = async::net::flag_e::SEND;
        } else if ( ss == state_e::recv ) {*/
          m_buffer.resize( maxBufferSize );
          m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
          m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
          m_flag = flag_e::RECV;
        //}
      } else {
        m_wsa_buf.buf += transferred;
        m_wsa_buf.len -= transferred;
      }

      return true;
    }
  };
}
}
