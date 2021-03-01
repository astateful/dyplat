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

#include "Request.hpp"
#include "Transform.hpp"

namespace astateful {
namespace algorithm {
  template<typename U, typename V> struct Generate;

  template<typename U, typename V> struct Parse;
}

namespace plugin {
  template<typename U, typename V> struct Compress;

  template<typename U, typename V> struct Expand;

  template<typename U> struct Pipe;
}

namespace protocol {
  //!
  template<typename U, typename V> struct Response {
    //!
    //!
    Response( const plugin::Pipe<U>& pipe,
              const plugin::Compress<U, V>& compress,
              const plugin::Expand<U, V>& expand,
              const algorithm::Generate<U, V>& generate,
              const algorithm::Parse<U, V>& parse,
              const Transform<U, V>& transform ) :
              m_algorithm(),
              m_pipe( pipe ),
              m_index( "output" ),
              m_compress( compress ),
              m_expand( expand ),
              m_parse( parse ),
              m_generate( generate ),
              m_transform( transform ) {}

    virtual ~Response() = default;

    //! This class never needs to be copied or moved because it is unique
    //! for a single connection only; its properties can in no way ever
    //! apply to other connections.
    //!
    Response( const Response& ) = delete;
    Response& operator=( const Response& ) = delete;
    Response& operator=( Response&& ) = delete;
    Response( Response&& ) = delete;

    //!
    //!
    const V& exec() const { return m_algorithm.exec; }
    void setExec( const V& exec ) { m_algorithm.exec = exec; }

    //!
    //!
    virtual bool create( std::vector<uint8_t>& output ) {
      return false;
    }

    //!
    //!
    bool create( const Request<U,V>& request,
                 const async::pipe::client::Engine& pipe,
                 std::vector<uint8_t>& output ) {
      // Selection of the initial state to optimize is determined by the
      // context. We only ever fail if we cannot produce any output at all.
      if ( !request.initAlgorithm( m_algorithm ) )
        return false;

      Key<V> key( m_algorithm.exec, "memoization", request.hash() );

      // Declare a local generator rather than a method since there should be
      // no need for contexts to override this procedure.
      auto generate = [ &, this ]() -> bool {
        int order = 0;
        m_generate( m_algorithm, order );

        // We only want to fail really hard if the memoization fails.
        //if ( !m_pipe.Write( key, m_compress( m_algorithm ) ) ) return false;

        // Ensure that there is always something in the buffer at the end.
        return finalize( request, pipe, output );
      };

      // Same as above, but for a parser. Critical here is that if not in debug
      // mode, the expand call can easily fail, in which case we need the
      // generator to kick in. TODO: Need to distinguish between a memoization
      // not existing and it existing but failing to be parsed.
      auto parse = [ &, this ]() -> bool {
        // If not in debugging, do a parse evaluation which uses a memoization,
        // if one was already generated. First expand the saved memoization into
        // the context.
        if ( !m_expand( m_algorithm, m_pipe.Read( key ) ) ) return generate();

        // .. and then do the actual evaluation.
        m_parse( m_algorithm );

        // Ensure that there is always something in the buffer at the end.
        return finalize( request, pipe, output );
      };

      // If debugging, only do a generate evaluation to avoid working with a
      // cached evaluation order that may not reflect current changes as they
      // are happening.
      if ( true ) return generate();

      return parse();
    }

    //!
    //!
    void clear() {
      m_algorithm.value.clear();
      m_algorithm.flux.clear();
      m_algorithm.state.clear();
    }

    //! This method is used to output an error result in case the astateful
    //! parse or evaluate methods fail. Note that it should never fail, so we
    //! are always guaranteed to return something.
    //!
    virtual void fail( std::vector<uint8_t>& output ) const {
      output.clear();
    };
  protected:
    //! The context representing an evaluation instance.
    algorithm::Context<U, V> m_algorithm;

    //! The pipe layer to use when doing data IO.
    const plugin::Pipe<U>& m_pipe;

    //! The flux name used to output the initial state data.
    const V m_index;

    //! The class which compresses an evaluation instance context.
    const plugin::Compress<U, V>& m_compress;

    //! The class which expands into an evaluation instance context.
    const plugin::Expand<U, V>& m_expand;

    //! The parse evaluator to use when reading a memoization.
    const algorithm::Parse<U, V>& m_parse;

    //!
    const algorithm::Generate<U, V>& m_generate;

    //! This method is used to parse the result of the initial state
    //! that was populated by either the parse or generate methods and
    //! generate a valid response, thus finalizing the output.
    //!
    bool finalize( const Request<U, V>& request,
                   const async::pipe::client::Engine& pipe,
                   std::vector<uint8_t>& output ) {
      // For brevity, store the state populating the value.
      auto& state = *m_algorithm.state.at( m_algorithm.exec );

      // Compute the value. Note that its totally valid to have a null
      // value; it means that output could not be generated.
      auto value( std::move( state( m_algorithm.value, m_index ) ) );
      if ( !value ) return false;

      output.clear();

      return m_transform( *value, m_algorithm.exec, output );
    }
  private:
    //!
    const Transform<U, V>& m_transform;
  };
}
}