#include "astateful/protocol/ws/Transform.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

namespace astateful {
namespace protocol {
namespace ws {
  bool Transform<bson::Serialize, std::string>::operator()(
    const bson::Serialize& input,
    const std::string& exec,
    std::vector<uint8_t>& output ) const {
    std::vector<uint8_t> body( *input.at( "body" ) );

    output.resize( 10 );

    output[0] |= 1 << 0; // fin is 1
    output[0] &= ~( 1 << 1 ); // rsv1 is 0
    output[0] &= ~( 1 << 2 ); // rsv2 is 0
    output[0] &= ~( 1 << 3 ); // rsv3 is 0
    output[0] &= ~( 1 << 4 ); // rsv3 is 0
    output[0] &= ~( 1 << 5 ); // text
    output[0] &= ~( 1 << 6 ); // text
    output[0] |= 1 << 7; // text

    int indexStartRawData = -1;

    if ( body.size() <= 125 )
    {
      output[1] = static_cast<uint8_t>( body.size() );
      indexStartRawData = 2;
    }
    else if ( body.size() >= 126 && body.size() <= 65535 )
    {
      output[1] = 126;
      output[2] = ( body.size() >> 8 ) & 255;
      output[3] = ( body.size() ) & 255;

      indexStartRawData = 4;
    }
    else
    {
      output[1] = 127;
      output[2] = ( body.size() >> 56 ) & 255;
      output[3] = ( body.size() >> 48 ) & 255;
      output[4] = ( body.size() >> 40 ) & 255;
      output[5] = ( body.size() >> 32 ) & 255;
      output[6] = ( body.size() >> 24 ) & 255;
      output[7] = ( body.size() >> 16 ) & 255;
      output[8] = ( body.size() >> 8 ) & 255;
      output[9] = ( body.size() ) & 255;

      indexStartRawData = 10;
    }

    output.resize( indexStartRawData + body.size() );

    for ( int i = 0; i < body.size(); ++i )
    {
      output[indexStartRawData + i] = body[i];
    }

    return true;
  }
}
}
}
