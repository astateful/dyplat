#include "State/Node.hpp"

#include "astateful/plugin/MIME.hpp"
#include "astateful/plugin/Iterate.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Datetime.hpp"
#include "astateful/bson/Element/Bool.hpp"
#include "astateful/bson/Element/Long.hpp"

#include <algorithm>

namespace astateful {
namespace protocol {
  template <> std::unique_ptr<bson::Serialize> StateNode<bson::Serialize, std::string>::operator()(
    const algorithm::value_t<bson::Serialize, std::string>& value,
    const std::string& flux ) const {
    const std::string& method = *( value.at( "method" )->at( "value" ) );

    if ( flux == "file" ) {
      if ( method == "GET" ) {
        // This is the MIME content type.
        const std::string content_type = m_data.contentType();
        if ( content_type == "" ) return nullptr;

        auto output = std::make_unique<bson::Serialize>();
        output->append( bson::ElementString( "path", m_data.path() ) );
        output->append( bson::ElementLong( "length", m_data.size() ) );
        output->append( bson::ElementString( "content_type", content_type ) );

        return output;
      }
    } else if ( flux == "path" ) {
      if ( method == "GET" ) {
        auto output = std::make_unique<bson::Serialize>();

        int i = 0;
        plugin::Iterate iterate( m_data.path() );

        while ( iterate ) {
          if ( iterate->displayable() ) {
            bson::Serialize node;
            auto error = bson::error_e::CLEAN;

            if ( !iterate->directory() ) {
              node.append( bson::ElementString( "extension", iterate->extension() ) );
              node.append( bson::ElementLong( "size", iterate->size() ) );
            }

            node.append( bson::ElementBool( "directory", iterate->directory() ) );
            node.append( bson::ElementDatetime( "created", iterate->created() ) );
            node.append( bson::ElementDatetime( "accessed", iterate->accessed() ) );
            node.append( bson::ElementDatetime( "altered", iterate->altered() ) );
            node.append( bson::ElementString( "name", iterate->name() ) );

            if ( !output->append( std::to_string( i ), node, error ) ) return nullptr;

            ++i;
          }

          ++iterate;
        }

        return output;
      }
    }

    return nullptr;
  }
}
}
