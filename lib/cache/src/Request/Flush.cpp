#include "Request/Flush.hpp"

namespace astateful {
namespace cache {
  RequestFlush::RequestFlush() :
    Request( "flush_all\r\n" ) {}
}
}
