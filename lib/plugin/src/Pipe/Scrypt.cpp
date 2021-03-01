#include "astateful/plugin/Pipe/Scrypt.hpp"
#include "astateful/plugin/Store.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"
#include "astateful/algorithm/Context.hpp"

namespace astateful {
namespace plugin {
  template<> std::unique_ptr<bson::Serialize> PipeScrypt<bson::Serialize>::Read(
    const Key<std::string>& key ) const {
    std::vector<uint8_t> encrypted( 0 );

    switch ( m_store.Get( key, encrypted ) ) {
    case store_e::STORAGE_BAD:
    case store_e::STORAGE_NOEXIST:
    case store_e::DATA_EMPTY:
    case store_e::DATA_BAD:
      return nullptr;
      break;
    case store_e::SUCCESS:
      {
        std::vector<uint8_t> decrypted( 0 );

        // Since we only ever write non epty data, we will always
        // have non empty encrypted data.
        switch ( decrypt( m_decrypt, encrypted, decrypted ) )
        {
        case crypto::scrypt::result_e::SUCCESS:
          return std::make_unique<bson::Serialize>( decrypted );
          break;
        default:
          return nullptr;
          break;
        }
      }
      break;
    }

    return nullptr;
  }

  template<> bool PipeScrypt<bson::Serialize>::Write(
    const Key<std::string>& key,
    const std::unique_ptr<bson::Serialize>& serialize ) const {
    if ( serialize ) {
      auto error = bson::error_e::CLEAN;
      std::vector<uint8_t> plain( 0 );

      if ( !serialize->bson( plain, error ) ) return false;

      std::vector<uint8_t> encrypted( 0 );

      // Encrypt the data which should never be empty if
      // we have a valid BSON serialization.
      switch ( encrypt( m_encrypt, plain, encrypted ) )
      {
      case crypto::scrypt::result_e::SUCCESS:
        {
          switch ( m_store.Set( key, encrypted ) )
          {
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
        }
        break;
      default:
        return false;
        break;
      }
    }

    return false;
  }
}
}