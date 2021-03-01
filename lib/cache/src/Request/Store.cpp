#include "Request/Store.hpp"

#include <sstream>

namespace astateful {
namespace cache {
  RequestStore::RequestStore( const std::string& key,
                              const std::string& type,
                              int flag,
                              int expiry,
                              const std::string& data,
                              uint64_t cas,
                              bool reply ) {
    std::stringstream ss;
    ss << type;
    ss << ' ';
    ss << key;
    ss << ' ';
    ss << flag;
    ss << ' ';
    ss << expiry;
    ss << ' ';
    ss << data.size();

    if ( type == "cas" ) {
      ss << ' ';
      ss << cas;
    }

    if ( !reply ) {
      ss << ' ';
      ss << "noreply";
    }

    ss << "\r\n";
    ss << data;
    ss << "\r\n";

    const std::string command = ss.str();

    m_buffer.insert( m_buffer.end(), command.begin(), command.end() );

  }
}
}
