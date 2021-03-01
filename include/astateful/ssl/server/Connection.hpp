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

#include "astateful/protocol/Connection.hpp"

#include "Handle.hpp"

namespace astateful {
namespace ssl {
namespace server {
  //!
  struct Context;

  //! Extend the connection class to operate on SSL data. This class inherits all
  //! the basic principles of the connection class but also defines the SSL
  //! methods which operate on the buffered data.
  template<typename U, typename V> struct Connection final :
    public protocol::Connection<U, V> {
    //! As a precaution, ensure that this class cannot be default constructed.
    //!
    Connection() = delete;

    //! Extent the definition of the default connection to include the SSL
    //! global context which is needed to create an SSL instance for this
    //! unique connection.
    //!
    Connection( const protocol::Context<U, V>& protocol,
                const async::net::Address& local,
                const async::net::Address& remote,
                Context& ssl_context ) :
                protocol::Connection<U, V>( protocol,
                local,
                remote,
                ssl_context.alpn() ),
                m_handle( ssl_context, maxBufferSize ) {}

    //! This class never needs to be copied or moved because it is unique
    //! for a single connection only; its properties can in no way ever
    //! apply to other connections.
    //!
    Connection( const Connection& ) = delete;
    Connection& operator=( const Connection& ) = delete;
    Connection& operator=( Connection && ) = delete;
    Connection( Connection&& ) = delete;
  protected:
    //!
    //!
    bool recvEvent( DWORD transferred,
                    const async::pipe::client::Engine& pipe ) override {
      m_buffer.resize( transferred );

      bool connection_closed = false;

      // Update the SSL context. After every update we can check if the
      // handshake is finished and if so, act accordingly.
      if ( !m_handle.update( m_buffer ) ) return false;

      if ( m_handle.ssl_handshake_finished() ) {
        // In the receive state, we expect that data will arrive so we
        // can try to decrypt it and then generate a response for it.
        if ( m_state == state_e::recv || m_state == state_e::upgrade ) {
          // Holds the decrypted data. We need to loop until we have nothing
          // left to read.
          std::vector<uint8_t> decrypted( maxBufferSize );
          bool responded = false;
          int read;

          while ( ( read = m_handle.read( decrypted ) ) > 0 ) {
            if ( m_request->update( &decrypted[0], read ) ) {
              std::vector<uint8_t> output;

              respond( output, pipe );

              m_request->clear();
              m_response->clear();

              auto written = m_handle.write( output );
              if ( written != output.size() ) return false;

              responded = true;
            }
          }

          //if ( responded ) m_state = state_e::send;

          /*int read = m_handle.read( decrypted );
          if ( read > 0 ) {
          } else if ( read == 0 ) {
            //auto x = handle.shutdownState();
            // SSL_RECEIVED_SHUTDOWN
            //if (x == 2) {
            //  auto y = handle.shutdown();
            //  return false;
            //}
          } else if (read == -1) {

            //auto x = m_handle.geterror(read);
          }*/
        } else if ( m_state == state_e::send ) {
          std::vector<uint8_t> output;

          m_response->create( output );

          auto written = m_handle.write( output );
          if ( written != output.size() ) return false;

          m_state = state_e::recv;
        }
      }

      if ( m_handle.flush( m_buffer ) ) {
        if ( m_buffer.size() > ULONG_MAX ) return false;

        m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
        m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
        m_flag = flag_e::SEND;
      } else {
        m_buffer.resize( maxBufferSize );
        m_wsa_buf.len = static_cast<ULONG>( maxBufferSize );
        m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
        m_flag = flag_e::RECV;
      }

      return true;
    }

    //!
    //!
    bool sendEvent( DWORD transferred,
                    const async::pipe::client::Engine& pipe ) override {
      if ( m_wsa_buf.len - transferred == 0 ) {
        if (m_handle.flush( m_buffer )) {
          if ( m_buffer.size() > ULONG_MAX ) return false;

          m_wsa_buf.len = static_cast<ULONG>( m_buffer.size() );
          m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
          m_flag = flag_e::SEND;
        } else {
          //auto y = m_handle.shutdownState();
          m_buffer.resize( maxBufferSize) ;
          m_wsa_buf.len = static_cast<ULONG>( maxBufferSize );
          m_wsa_buf.buf = reinterpret_cast<char *>( m_buffer.data() );
          m_flag = flag_e::RECV;
        }
      } else {
        m_wsa_buf.buf += transferred;
        m_wsa_buf.len -= transferred;
      }

      return true;
    }
  private:
    //!
    Handle m_handle;
  };
}
}
}
