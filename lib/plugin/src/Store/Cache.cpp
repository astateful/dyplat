#include "astateful/plugin/Store/Cache.hpp"
#include "astateful/cache/Context.hpp"
#include "astateful/plugin/Key.hpp"

namespace astateful {
namespace plugin {
  StoreCache::StoreCache( const cache::Context& cache ) :
    m_cache( cache ),
    Store() {}

  store_e StoreCache::Get( const Key<std::string>& key,
                           std::vector<uint8_t>& output ) const {
    auto collapsed_key = key.prefix + key.value + key.hash;
    auto data = cache::get( m_cache, collapsed_key );
    if ( data == "" ) return store_e::STORAGE_BAD;

    output.assign( data.begin(), data.end() );

    return store_e::SUCCESS;
  }

  store_e StoreCache::Set( const Key<std::string>& key,
                          const std::vector<uint8_t>& input ) const {
    if ( input.size() == 0 ) { return store_e::DATA_EMPTY; }

    auto collapsed_key = key.prefix + key.value + key.hash;
    std::string data = { input.begin(), input.end() };

    if ( cache::set( m_cache, collapsed_key, data ) == cache::result_e::NOT_STORED )
      return store_e::STORAGE_BAD;

    return store_e::SUCCESS;
  }
}
}
