#include "astateful/byte/h2/hpack/HeaderField.hpp"

namespace astateful {
namespace byte {
namespace h2 {
namespace hpack {
  HeaderField::HeaderField( const std::string& name_,
                            const std::string& value_ ) :
    name( name_ ),
    value( value_ ) {}
}
}
}
}