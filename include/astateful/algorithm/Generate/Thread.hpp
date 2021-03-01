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
#include "Task.hpp"

#include <thread>

namespace astateful {
namespace algorithm {
  //! The threaded approach uses a thread for each state, with each state waiting
  //! on values that it needs to be computed. This avoids using an infinite loop
  //! or recursion at the expense of threading complexity. The major advantage
  //! here is that states can be evaluated in parallel if they correspond roughly
  //! to the same level in the tree.
  template<typename U, typename V> struct GenerateThread : public Generate<U, V> {
    //! Construct a threaded generate with the persistence layer used to determine
    //! if a value needs to be computed or not.
    //!
    //! @param persist The persistence layer to use when looking up a value.
    //!
    GenerateThread( const Persist<U, V>& persist ) : Generate<U, V>( persist ) {}

    //! This evaluator just sets up the threading and create a separate task for
    //! each state. The data declared here is common to all states.
    //!
    //! @param context The current evaluation context to evaluate.
    //! @param order The order in which the current value was computed.
    //!
    void operator()( Context<U, V>& context, int& order ) const override {
      std::vector<std::unique_ptr<Task<U, V>>> task;
      std::vector<std::thread> thread;
      size_t num_thread = context.state.size() - 1;
      condition_t<V> condition;
      std::mutex mutex;
      size_t i = 0;

      thread.resize( num_thread );

      for ( const auto& flux : context.flux ) {
        // Do not include any fluxes from the executing state since they
        // pull. We only want the fluxes which are pushing.
        if ( flux.first == context.exec ) continue;

        // Make sure that there is a state that supports populating this
        // flux. It can happen that the particular state does not exist.
        if ( context.state.find( flux.first ) == end( context.state ) )
          continue;

        // Hate using a unique_ptr here but we need to insert elements
        // using std::condition_variable which isnt copyable...or movable!?
        condition[flux.second] = std::make_unique<std::condition_variable>();
      }

      for ( const auto& state : context.state ) {
        // Wrap this in an if in order to preserve
        // the uniform increment of i.
        if ( state.first != context.exec ) {
          task.emplace_back( std::make_unique<Task<U, V>>(
                                state.first,
                                context,
                                order,
                                mutex,
                                condition,
                                m_persist ) );

          thread[i] = std::thread( exec_task<U, V>, task[i].get() );

          ++i;
        }
      }

      for ( i = 0; i < num_thread; ++i ) thread[i].join();
    }
  };
}
}
