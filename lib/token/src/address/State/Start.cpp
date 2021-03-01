#include "autogen/token/address/State/Start.hpp"

namespace astateful {
namespace token {
namespace address {
  state_e StateStart::operator()( Context&, wchar_t ) {
    return state_e::Done;
  }
}
}
}