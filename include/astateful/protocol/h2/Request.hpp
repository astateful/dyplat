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

#include "astateful/byte/h2/Context.hpp"
#include "astateful/byte/h2/Machine.hpp"
#include "astateful/byte/h2/FrameHeader.hpp"

#include "../Request.hpp"

#include "Windows.h"

namespace astateful {
namespace mongo {
  //!
  struct Context;
}

namespace protocol {
namespace h2 {
  //!
  //!
  template<typename U, typename V> struct Request : 
    public protocol::Request<U, V> {
    //!
    //!
    Request( const byte::h2::Machine& machine,
             const std::wstring& public_html,
             const mongo::Context& mongo,
             const std::wstring& script_path,
             const std::wstring& memoize_path ) :
      protocol::Request<U, V>(),
      m_machine( machine ),
      //m_h2( m_machine.initialState() ),
      m_public_html( public_html ),
      m_mongo( mongo ),
      m_script_path( script_path ),
      m_memoize_path( memoize_path ) {}

    //!
    //!
    void clear() override;

    //!
    //!
    V hash() const override;

    bool update( const uint8_t * data, int length ) {
       auto initial = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

       if ( length == 24) {
         bool found = true;
         for ( int i = 0; i < 24; ++i) {
           if (initial[i] != data[i]) {
             found = false;
             break;
           }
         }

         if (found) {
           m_h2.haveConnectionPreface = true;
         }

       } else {
         byte::h2::FrameHeader h( data, length );

         auto num_packets = ( length - 9 ) / 6;

         int offset = 9;

         // SETTINGS FRAME
         if (h.get_type() == 4) {
           for (int i = 0; i < num_packets; ++i) {
             auto start = ( i * 6) + offset;

             std::vector<uint8_t> iden;
             iden.push_back( data[start] );
             iden.push_back( data[start+1] );

             auto identifier = ntohs( *reinterpret_cast<const uint32_t*>( iden.data() ) );

             std::vector<uint8_t> pl;
             pl.push_back ( data[start+2] );
             pl.push_back ( data[start+3] );
             pl.push_back ( data[start+4] );
             pl.push_back ( data[start+5] );
             pl.push_back ( 0 );
             pl.push_back ( 0 );
             pl.push_back ( 0 );
             pl.push_back ( 0 );

             auto value = ntohl( *reinterpret_cast<const uint32_t*>( pl.data() ) );

             int halt = 0;
           }
         } else if ( h.get_type() == 8 ) {
           //WINDOW_UPDATE
           int offset = 9;

           std::vector<uint8_t> pl;
           pl.push_back( data[offset] );
           pl.push_back( data[offset + 1] );
           pl.push_back( data[offset + 2] );
           pl.push_back( data[offset + 3] );
           pl.push_back( 0 );
           pl.push_back( 0 );
           pl.push_back( 0 );
           pl.push_back( 0 );

           auto window_size_increment = ntohl( *reinterpret_cast<const uint32_t*>(pl.data()) );

           int halt = 0;

         } else if ( h.get_type() == 1) {
           /*
          END_STREAM (0x1):
          Bit 0 being set indicates that the header block (Section 4.3) is the last that the endpoint will send for the identified stream.

          A HEADERS frame carries the END_STREAM flag that signals the end of a stream. However, a HEADERS frame with the END_STREAM flag set can be followed by CONTINUATION frames on the same stream. Logically, the CONTINUATION frames are part of the HEADERS frame.
          */
           if ( h.get_flags() && ( 1 << 0 ) ) {
             int halt = 0;
           }

           /*
           END_HEADERS (0x4):
          Bit 2 being set indicates that this frame contains an entire header block (Section 4.3) and is not followed by any CONTINUATION frames.

          A HEADERS frame without the END_HEADERS flag set MUST be followed by a CONTINUATION frame for the same stream. A receiver MUST treat the receipt of any other type of frame or a frame on a different stream as a connection error (Section 5.4.1) of type PROTOCOL_ERROR.
          */
           if ( h.get_flags() && ( 1 << 2 )) {
             int halt = 0;
           }

           /*
            PADDED (0x8):
            Bit 3 being set indicates that the Pad Length field and any padding that it describes is present.
            */
           if ( h.get_flags() && ( 1 << 3 )) {
             int halt = 0;
           }

           /*
            PRIORITY (0x20):
            Bit 5 being set indicates that the Exclusive Flag (E), Stream Dependency, and Weight fields are present; see Section 5.3.
           */
           if ( h.get_flags() && ( 1 << 5 )) {
             int halt = 0;
           }
         }
       }

       int halt = 0;

       return true;

    }

    //! Provide a way for a client to choose which executing state to select
    //! based on data provided by the context.
    //!
    //! @param context The context representing an evaluation instance.
    //! @return False if the domain index could not be properly obtained.
    //!
    bool initAlgorithm( algorithm::Context<U,V>& algorithm ) const override;
  private:
    //! The FSM used to evaluate the HTTP2 request.
    const byte::h2::Machine& m_machine;

    //! The machine context used to store parsed HTTP2 data.
    token::h2::Context m_h2;

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
