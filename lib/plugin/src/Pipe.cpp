#include "astateful/plugin/Pipe.hpp"
#include "astateful/plugin/Store.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"

namespace astateful {
namespace plugin {
  template<> std::unique_ptr<bson::Serialize> Pipe<bson::Serialize>::Read(
    const Key<std::string>& key ) const {
    std::vector<uint8_t> plain( 0 );

    switch ( m_store.Get( key, plain ) ) {
    case store_e::STORAGE_BAD:
    case store_e::STORAGE_NOEXIST:
    case store_e::DATA_EMPTY:
    case store_e::DATA_BAD:
      return nullptr;
      break;
    case store_e::SUCCESS:
      return std::make_unique<bson::Serialize>( plain );
      break;
    }

    return nullptr;
  }

  template<> bool Pipe<bson::Serialize>::Write(
    const Key<std::string>& key,
    const std::unique_ptr<bson::Serialize>& serialize ) const {
    std::vector<uint8_t> plain( 0 );

    if ( serialize ) {
      auto error = bson::error_e::CLEAN;
      if ( !serialize->bson( plain, error ) ) return false;
    }

    switch ( m_store.Set( key, plain ) ) {
    case store_e::STORAGE_BAD:
    case store_e::STORAGE_NOEXIST:
    case store_e::DATA_EMPTY:
    case store_e::DATA_BAD:
      return false;
      break;
    case store_e::SUCCESS:
      return true;
      break;
    }

    return false;
  }
}
}
