#include "Request.hpp"

namespace astateful {
namespace cache {
  Request::Request( const std::string& buffer ) :
    async::net::client::Request( { buffer.begin(), buffer.end() } ) {}
}
}
