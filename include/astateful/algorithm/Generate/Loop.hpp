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

#include "../Generate.hpp"

namespace astateful {
namespace algorithm {
  //! The loop approach basically uses a while loop to approximate recursion.
  //! So rather than recurse to compute a value that another value depends on,
  //! another iteration of the loop will ensure that the value will eventually
  //! be computed.
  template<typename U, typename V> struct GenerateLoop : public Generate<U, V> {
    //! Construct a loop generate with the persistence layer used to determine
    //! if a value needs to be computed or not.
    //!
    //! @param persist The persistence layer to use when looking up a value.
    //!
    GenerateLoop( const Persist<U, V>& persist ) : Generate<U, V>( persist ) {}

    //! Set up our looping structure. At the end of the loop we check to see if
    //! all values required by the initial state were found.
    //!
    //! @param context The current evaluation context to evaluate.
    //! @param order The order in which the current value was computed.
    //!
    void operator()( Context<U, V>& context, int& order ) const override {
      bool finished = false;
      const auto& flux = context.flux;

      while ( !finished ) {
        for ( const auto& i : flux ) {
          if ( i.first == context.exec ) continue;

          bool evaluate = true;

          for ( const auto& dependency : i.second.dependency() ) {
            if ( context.value.find( dependency ) == end( context.value ) ) {
              evaluate = false;
              break;
            }
          }

          if ( evaluate ) {
            if ( context.value.find( i.second ) == end( context.value ) ) {
              context.value[i.second] = {
                i.first,
                order,
                m_persist( context, i.first, i.second )
              };

              ++order;
            }
          }
        }

        finished = true;

        for ( auto i = flux.lower_bound( context.exec ); i != flux.upper_bound( context.exec ); ++i ) {
          if ( context.value.find( i->second ) == end( context.value ) ) {
            finished = false;
            break;
          }
        }
      }
    }
  };
}
}
