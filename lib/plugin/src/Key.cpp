#include "astateful/plugin/Key.hpp"
#include "astateful/crypto/md5.hpp"

namespace astateful {
namespace plugin {
  Key<std::string>::Key( const std::string& prefix_,
                         const std::string& value_,
                         const std::string& plain ) :
    prefix( prefix_ ),
    value( value_ ),
    hash( crypto::md5( plain ) ) {}
}
}