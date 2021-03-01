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

#include "../Context.hpp"

#include <mutex>
#include <condition_variable>

namespace astateful {
namespace algorithm {
  //! Define a managed type for our condition container.
  template<typename V> using condition_t = std::unordered_map<V, std::unique_ptr<std::condition_variable>>;

  //! A task performs the actual threaded functionality for each state.
  template<typename U, typename V> struct Task {
    //! Construct a task with the global state information.
    //!
    //! @param index The state index which is not stored with the state.
    //! @param context The current evaluation context to evaluate.
    //! @param order The order in which the current value was computed.
    //! @param mutex Locks shared data during simultaneous thread access.
    //! @param condition Used by the state to wait on particular values.
    //! @param persist The persistence layer to use when looking up a value.
    //!
    Task( const V& index,
          Context<U, V>& context,
          int& order,
          std::mutex& mutex,
          condition_t<V>& condition,
          const Persist<U, V>& persist ) :
      m_index( index ),
      m_order( order ),
      m_mutex( mutex ),
      m_context( context ),
      m_condition( condition ),
      m_persist( persist ) {}

    //! This method operates on a single state by first traversing through
    //! its assigned fluxes and before creating a flux, waits on other fluxes
    //! to have values that the created flux might depend on.
    //!
    void operator()() {
      auto& value = m_context.value;
      const auto& flux = m_context.flux;
      std::unique_lock<std::mutex> lock( m_mutex, std::defer_lock );

      for ( auto j = flux.lower_bound( m_index ); j != flux.upper_bound( m_index ); ++j ) {
        for ( const auto& dependency : j->second.dependency() ) {
          if ( m_condition.find( dependency ) == end( m_condition ) ) continue;

          lock.lock();

          m_condition[dependency]->wait( lock, [&] {
            if ( m_context.value.find( dependency ) == end( m_context.value ) ) return false;

            return true;
          } );

          lock.unlock();
        }

        lock.lock();

        value[j->second] = {
          m_index,
          m_order,
          m_persist( m_context, m_index, j->second )
        };

        ++m_order;

        lock.unlock();

        // Awake any states which are waiting for this flux value.
        m_condition[j->second]->notify_all();
      }
    }
  private:
    //! The state index which is not stored with the state.
    V m_index;

    //! The order in which the current value was computed.
    int& m_order;

    //! Locks shared data during simultaneous thread access.
    std::mutex& m_mutex;

    //! The current evaluation context to evaluate.
    Context<U, V>& m_context;

    //! Used by the state to wait on particular values.
    condition_t<V>& m_condition;

    //! The persistence layer to use when looking up a value.
    const Persist<U, V>& m_persist;
  };

  //! This method operates on a single state by traversing through its
  //! assigned fluxes and ensuring that all of its dependencies are satisfied.
  //! Note that each method is running for each state, so all the threads
  //! depend on each other.
  //!
  //! @param task The the task describing this particular state.
  //!
  template<typename U, typename V> void exec_task( Task<U, V> * task ) {
    ( *task )();
  }
}
}
