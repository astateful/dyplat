#include "State/Negotiate.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/Binary/Generic.hpp"

#include "astateful/byte/h2/Context.hpp"
#include "astateful/byte/h2/FrameHeader.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateNegotiate<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux != "output" ) return nullptr;

    auto output = std::make_unique<bson::Serialize>();

    if (m_h2.haveConnectionPreface ) {
      int halt = 0;

      byte::h2::FrameHeader ack_settings_fh( 0x0, 0x4, 0x0, 0x0 );

      auto j = ack_settings_fh.write_to_buffer();

      output->append( bson::ElementBinaryGeneric( "data", j ) );
    }

    return output;
  }
}
}
