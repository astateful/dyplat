#include "astateful/plugin/Store/Mongo.hpp"
#include "astateful/plugin/Key.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Binary/Generic.hpp"

namespace astateful {
namespace plugin {
  StoreMongo::StoreMongo( const mongo::Context& mongo,
                          const std::string& collection ) :
    m_mongo( mongo ),
    m_collection( collection ),
    Store() {}

  store_e StoreMongo::Get( const Key<std::string>& key,
                           std::vector<uint8_t>& output ) const {
    const auto combined = key.prefix + key.value + key.hash;
    auto error = bson::error_e::CLEAN;

    bson::Serialize query;
    query.append( bson::ElementString( "context", combined ) );

    bson::Serialize result;
    if ( mongo::find_one( m_mongo, m_collection, query, result, error ) ) {
      output = result["data"];
      
      return store_e::SUCCESS;
    }

    return store_e::STORAGE_BAD;
  }

  store_e StoreMongo::Set( const Key<std::string>& key,
                           const std::vector<uint8_t>& input ) const {
    if ( input.size() == 0 ) return store_e::DATA_EMPTY;

    const auto combined = key.prefix + key.value + key.hash;
    auto error = bson::error_e::CLEAN;

    bson::Serialize document;
    document.append( bson::ElementString( "context", combined ) );
    document.append( bson::ElementBinaryGeneric( "data", input ) );

    if ( !mongo::insert( m_mongo, m_collection, document, error ) )
      return store_e::DATA_BAD;

    return store_e::SUCCESS;
  }
}
}
