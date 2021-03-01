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

#include <map>
#include <memory>

#include "State.hpp"
#include "Table.hpp"

namespace astateful {
namespace token {
  //!
  template<typename state_t, typename context_t,
    typename value_t> struct Machine {
    //!
    //!
    Machine() = default;
    virtual ~Machine() = default;

    //!
    //!
    virtual state_t initialState() const = 0;

    //!
    //!
    virtual state_t successState() const = 0;

    //!
    //!
    bool operator()( context_t& context, state_t& state,
                     const value_t value ) const {
      auto p_entry = beginTable();
      auto p_table_end = endTable();
      bool state_found = false;

      while ( !state_found && ( p_entry != p_table_end ) ) {
        if ( p_entry->current_state_id == state ) {
          if ( p_entry->transition_letter == value ) {

            state = ( *m_state.at( p_entry->next_state_id ) )( context, value );

            state_found = true;

            break;
          }
        }
        ++p_entry;
      }

      return state_found;
    }
  protected:
    //!
    using class_t = token::State<state_t, context_t, value_t>;

    //!
    std::map<state_t, std::unique_ptr<class_t>> m_state;

    //!
    virtual const Table<state_t, value_t> * beginTable() const = 0;

    //!
    virtual const Table<state_t, value_t> * endTable() const  = 0;
  };
}
}
