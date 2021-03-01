#pragma once

#include <string>

namespace astateful {
namespace byte {
namespace h2 {
namespace hpack {
  const static int HEADER_ENTRY_OVERHEAD = 32;

  struct HeaderField {


    std::string name;
    std::string value;

    HeaderField( const std::string& name, const std::string& value );

    int size() const { return name.length() + value.length() + 32; }




  };
}
}
}
}