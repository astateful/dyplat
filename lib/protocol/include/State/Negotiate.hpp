#pragma once

#include "astateful/algorithm/State.hpp"

namespace astateful {
namespace token {
namespace h2 {
  //!
  struct Context;
}
}

namespace protocol {
  //!
  template<typename U, typename V> struct StateNegotiate : public algorithm::State<U, V> {
    //! constructor
    //!
    //! @param 
    //!
    StateNegotiate( const token::h2::Context& h2 ) :
      algorithm::State<U, V>(),
      m_h2( h2 )
    {}

    //! Return a new unknown based on the flux index as well as the values
    //! provided by the given value map.
    //!
    std::unique_ptr<U> operator()( const algorithm::value_t<U, V>&, const V& ) const override;
  private:
    //! The internal data which is outputted by this state.
    const token::h2::Context& m_h2;
  };
}
}