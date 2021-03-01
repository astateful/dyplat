#include "State/File.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/bson/Element/Long.hpp"

namespace astateful {
namespace protocol {
  template<> std::unique_ptr<bson::Serialize> StateFile<bson::Serialize, std::string>::operator() (
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    const std::string& method = *( value.at( "method" )->at( "value" ) );

    if ( flux == "output" ) {
      if ( method == "GET" ) {
        if ( !value.at( "file" ) ) return nullptr;

        auto error = bson::error_e::CLEAN;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementString( "version", L"HTTP/1.1" ) );
        output->append( bson::ElementInt( "code", 200 ) );
        output->append( bson::ElementString( "reason", L"OK" ) );

        int64_t content_length = *value.at( "file" )->at( "length" );
        std::string content_type = *value.at( "file" )->at( "content_type" );
        std::string path = *value.at( "file" )->at( "path" );

        bson::Serialize header;
        header.append( bson::ElementLong( "Content-Length", content_length ) );
        header.append( bson::ElementString( "Content-Type", content_type ) );
        header.append( bson::ElementString( "Connection", "keep-alive" ) );

        output->append( bson::ElementString( "path", path ) );
        if ( !output->append( "header", header, error ) ) return nullptr;

        return output;
      }
    }

    return nullptr;
  }
}
}
