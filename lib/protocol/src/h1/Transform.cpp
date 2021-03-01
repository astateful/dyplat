#include "astateful/protocol/h1/Transform.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Iterate.hpp"
#include "astateful/bson/Element.hpp"

#include <fstream>

namespace astateful {
namespace protocol {
namespace h1 {
namespace {
  bool write_header( const bson::Serialize& input,
                     std::vector<uint8_t>& output ) {
    // Force type deduction here.
    std::string version( *input.at( "version" ) );
    std::string code( *input.at( "code" ) );
    std::string reason( *input.at( "reason" ) );

    output.insert( output.end(), version.begin(), version.end() );
    output.push_back( 32 );
    output.insert( output.end(), code.begin(), code.end() );
    output.push_back( 32 );
    output.insert( output.end(), reason.begin(), reason.end() );
    output.push_back( '\r' );
    output.push_back( '\n' );

    std::vector<uint8_t> header( *input.at( "header" ) );
    bson::Iterate iterate( header );
    std::string tmp( "" );

    while ( iterate ) {
      auto& key = iterate->key();

      output.insert( output.end(), key.begin(), key.end() );
      output.push_back( ':' );
      output.push_back( ' ' );

      tmp = *iterate;

      output.insert( output.end(), tmp.begin(), tmp.end() );
      output.push_back( '\r' );
      output.push_back( '\n' );

      ++iterate;
    };

    output.push_back( '\r' );
    output.push_back( '\n' );

    return true;
  }
}

  bool Transform<bson::Serialize, std::string>::operator()(
    const bson::Serialize& input,
    const std::string& exec,
    std::vector<uint8_t>& output ) const {
    if ( !write_header( input, output ) ) return false;

    if ( exec == "file" ) {
      // Need to force type deduction here.
      const std::string path = *input.at( "path" );

      // The validity of the file path should have already
      // been checked when the request was created.
      std::ifstream stream( path, std::ios::in | std::ios::binary );
      stream.seekg( 0, std::ios::end );
      auto body_size = stream.tellg();
      stream.seekg( 0, std::ios::beg );
      std::vector<char> body( body_size );

      // TODO: Does this still work for empty files?
      if ( stream.read( body.data(), body_size ) )
        output.insert( output.end(), body.begin(), body.end() );
    } else {
      // TODO: Somehow avoid this copy...
      std::vector<uint8_t> body( *input.at( "body" ) );

      output.insert( output.end(), body.begin(), body.end() );
    }

    return true;
  }
}
}
}
