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

#include <memory>
#include <unordered_map>

namespace astateful {
namespace algorithm {
  //! The value class represents the computed flux value.
  template <typename U, typename V> struct Value final {
    //! Provide a default constructor so that our value class can be used
    //! within the standard containers.
    //!
    Value() : m_create(), m_order( 0 ) {}

    //! Note that a value which has been evaluated can never have an order
    //! less than 0 since the order defined within the evaluator is
    //! defined to start at 0. Additionally, the create parameter will always
    //! be valid since data was created from the state at that index.
    //!
    //! @param create The index of the state which creates this value.
    //! @param order The global order in which this value was generated.
    //! @param data The data which has been computed by a state. Can be null.
    //!
    Value( V create, int order, std::unique_ptr<U>& data ) :
      m_create( create ),
      m_order( order ),
      m_data( std::move( data ) ) {}

    //! This constructor is required when reading memoized data...we never
    //! store data along with the memoization, just the order of the values
    //! so we need to set data at a later point in time.
    //!
    //! @param create The index of the state which creates this value.
    //! @param order The global order in which this value was generated.
    //!
    Value( V create, int order ) :
      m_create( create ),
      m_order( order ) {}

    //! A value never needs to be copied or moved since a value represents a
    //! uniquely computed value designed to fulfill the requirements of a
    //! state. So a copy of a value serves no purpose.
    //!
    Value( const Value& ) = delete;
    Value& operator=( const Value& ) = delete;

    //! Provide a move constructor so that our value can be used within the
    //! standard containers.
    //!
    //! @param rhs The class whose contents we are moving from.
    //!
    Value( Value&& rhs ) :
      m_data( std::move( rhs.m_data ) ),
      m_create( std::move( rhs.m_create ) ),
      m_order( std::move( rhs.m_order ) ) {}

    //! Provide a move assignment operator so that our value can be used within
    //! the standard containers.
    //!
    //! @param rhs The class whose contents we are moving from.
    //!
    Value& operator=( Value && rhs ) {
      if ( this != &rhs ) {
        m_data = std::move( rhs.m_data );
        m_create = std::move( rhs.m_create );
        m_order = std::move( rhs.m_order );
      }

      return *this;
    }

    //! Provide an overloaded operator for convenient access to the underlying
    //! value data. Use this if the value data has the potential to be null.
    //!
    //! @return A pointer to the internal computed value data.
    //!
    const U * operator->() const { return m_data.get(); }

    //! Provide an overloaded dereference operator for convenient access to the
    //! underlying value data. Use this method if the value data should not be nullptr.
    //!
    //! @return A pointer to the internal computed value data.
    //!
    const U& operator*() const { return *m_data; }

    //! Provide a convenient way to determine if the internal value data is
    //! empty or not without querying the internal data directly.
    //!
    //! @return True if the internal data is not empty, false otherwise.
    //!
    operator bool() const { return static_cast<bool>( m_data ); }

    //! Set the data for this value. This is data which has been computed by a
    //! state. The value will assume ownership of this data. We provide this method
    //! because in the case of parse evaluation the data will be set after it
    //! has been created.
    //!
    //! @param data The data that the value will assume ownership over.
    //!
    void SetData( std::unique_ptr<U>& data ) {
      m_data = std::move( data );
    }

    //! Get the index of the state which has created the data for this
    //! value. As with SetCreate, this index is used to call the correct
    //! state which populates the value data.
    //!
    //! @return The index of the state which creates the value data.
    //!
    const V& create() const { return m_create; }

    //! Return the evaluation order of this value. Note that a value with
    //! a negative order indicates the value was never evaluated.
    //!
    //! @return The output order to populate from this value.
    //!
    const int& order() const { return m_order; }
  private:
    //! The data which has been computed by a state. Can be null.
    std::unique_ptr<U> m_data;

    //! The index of the state which creates this value.
    V m_create;

    //! The global order in which this value was generated.
    int m_order;
  };

  //! Define a managed type for our value container.
  template<typename U, typename V> using value_t = std::unordered_map<V, Value<U, V>>;
}
}
