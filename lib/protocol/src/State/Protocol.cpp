#include "State/Protocol.hpp"

#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/crypto/base64.hpp"

#include <openssl/sha.h>

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateProtocol<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    const std::string& method = *( value.at( "method" )->at( "value" ) );

    if ( flux == "ws" ) {
      if ( method == "GET" ) {
        // Retrieve the request header which contains the Websocket key.
        const auto& request_headers = value.at( "header" );
        if ( request_headers->at( "Sec-WebSocket-Key" ) == nullptr ) return nullptr;

        std::string key = *request_headers->at( "Sec-WebSocket-Key" );
        key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

        SHA_CTX context;
        std::vector<uint8_t> digest( SHA_DIGEST_LENGTH, 0 );

        if ( !SHA1_Init( &context ) ) return nullptr;
        if ( !SHA1_Update( &context, key.c_str(), key.length() ) ) return nullptr;
        if ( !SHA1_Final( digest.data(), &context ) ) return nullptr;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementString( "Upgrade", "websocket" ) );
        output->append( bson::ElementString( "Connection", "Upgrade" ) );
        output->append( bson::ElementString( "Sec-WebSocket-Accept", crypto::base64encode( digest ) ) );

        return output;
      }
    }

    return nullptr;
  }
}
}
