#include "autogen/token/h1/State/Line.hpp"

namespace astateful {
namespace token {
namespace h1 {
  state_e StateLine::operator()( Context&, uint8_t ) {
    return state_e::Line;
  }
}
}
}