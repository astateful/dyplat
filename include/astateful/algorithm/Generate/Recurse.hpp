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
  //! The top-down approach uses recursion and memoization. Once we have found an
  //! unknown we store it in a table to speed up lookups. But we still need to
  //! visit the same state more than once, and perform recursion so this is not
  //! the ideal method to use for state evaulation in some cases, if the
  //! dependencies are too complex.
  template<typename U, typename V> struct GenerateRecurse : public Generate<U, V> {
    //! Construct a recurse generate with the persistence layer used to determine
    //! if a value needs to be computed or not.
    //!
    //! @param persist The persistence layer to use when looking up a value.
    //!
    GenerateRecurse( const Persist<U, V>& persist ) : Generate<U, V>( persist ) {}

    //! Setup the recursion approach by only recursing over all those elements
    //! which belong to the executing state. The alternative approach of not
    //! doing the loop first is much too messy.
    //!
    //! @param context The current evaluation context to evaluate.
    //! @param order The order in which the current value was computed.
    //!
    void operator()( Context<U, V>& context, int& order ) const override {
      const auto& flux = context.flux;

      for ( auto i = flux.lower_bound( context.exec ); i != flux.upper_bound( context.exec ); ++i ) {
        exec_task( i->second, context, order );
      }
    }
  private:
    //! This is the method which performs the actual recursion. For the given
    //! flux, see if its dependencies map to values which already exist. If not,
    //! compute those values. Repeat.
    //!
    //! @param flux The flux whose value we need to compute for a state.
    //! @param context The current evaluation context to evaluate.
    //! @param order The order in which the current value was computed.
    //!
    void exec_task( const V& flux, Context<U, V>& context, int& order ) const {
      for ( const auto& i : context.flux ) {
        if ( flux == V( i.second ) && i.first != context.exec ) {
          for ( const auto& dependency : i.second.dependency() ) {
            exec_task( dependency, context, order );
          }

          if ( context.value.find( flux ) == end( context.value ) ) {
            context.value[flux] = {
              i.first,
              order,
              m_persist( context, i.first, flux )
            };

            ++order;
          }
        }
      }
    }
  };
}
}
