#include "State/Script.hpp"

#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateScript<bson::Serialize, std::string>::operator()(
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    if ( flux == "output" ) {
      double ( *FP )( double ) = ( double ( * )( double ) )( intptr_t )FPtr;
      double d = FP ( 10 );

      auto output = std::make_unique<bson::Serialize>();
      output->append( bson::ElementString( "version", L"HTTP/1.1" ) );
      output->append( bson::ElementInt( "code", 200 ) );
      output->append( bson::ElementString( "reason", L"OK" ) );

      std::string body = std::to_string( d );

      auto error = bson::error_e::CLEAN;

      bson::Serialize header;
      header.append( bson::ElementInt( "Content-Length", body.size() ) );
      header.append( bson::ElementString( "Content-Type", L"text/html; charset=UTF-8" ) );
      header.append( bson::ElementString( "Connection", "keep-alive" ) );

      if ( !output->append( "header", header, error ) ) return nullptr;

      output->append( bson::ElementString( "body", body ) );

      return output;
    }

    return nullptr;
  }
}
}
