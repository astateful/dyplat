#include "astateful/plugin/Store/File.hpp"
#include "astateful/algorithm/Context.hpp"
#include "astateful/plugin/Key.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include "Shlwapi.h"

#include <fstream>
#include <codecvt>
#include <vector>

namespace astateful {
namespace plugin {
  namespace {
    bool create_directory_tree( const std::wstring& path,
                                const Key<std::string>& key ) {
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

      std::wstring absolute( path );
      absolute += L"\\";
      absolute += convert.from_bytes( key.prefix );

      if ( !CreateDirectoryW( absolute.c_str(), nullptr ) ) {
        switch ( GetLastError() )
        {
        case ERROR_PATH_NOT_FOUND:
          return false;
          break;
        case ERROR_INVALID_NAME:
          return false;
          break;
        }
      }

      absolute += L"\\";
      absolute += convert.from_bytes( key.hash );

      if ( !CreateDirectoryW( absolute.c_str(), nullptr ) ) {
        switch ( GetLastError() )
        {
        case ERROR_PATH_NOT_FOUND:
          return false;
          break;
        case ERROR_INVALID_NAME:
          return false;
          break;
        }
      }

      return true;
    }

    std::wstring get_abs_file_path( const std::wstring& path,
                                    const Key<std::string>& key ) {
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

      std::wstring absolute( path );
      absolute += L"\\";
      absolute += convert.from_bytes( key.prefix );
      absolute += L"\\";
      absolute += convert.from_bytes( key.hash );
      absolute += L"\\";
      absolute += convert.from_bytes( key.value );
      absolute += L".bin";

      return absolute;
    }
  }

  store_e StoreFile::Get( const Key<std::string>& key,
                          std::vector<uint8_t>& output ) const {
    const auto file_path = get_abs_file_path( m_path, key );

    // Use Windows to make sure that the file path exists.
    if ( !PathFileExistsW( file_path.c_str() ) )
      return store_e::STORAGE_NOEXIST;

    std::ifstream stream( file_path, std::ios::in | std::ios::binary );
    if ( !stream.good() ) return store_e::STORAGE_BAD;

    // Determine the size of the data to retrieve (make sure to rewind).
    stream.seekg( 0, std::ios::end );
    output.resize( static_cast<int>( stream.tellg() ) );

    // Make sure that the output is never empty.
    if ( output.size() == 0 ) return store_e::DATA_EMPTY;

    // Read the data into the output.
    stream.seekg( 0, std::ios::beg );
    stream.read( reinterpret_cast<char *>( output.data() ), output.size() );

    return store_e::SUCCESS;
  }

  store_e StoreFile::Set( const Key<std::string>& key,
                          const std::vector<uint8_t>& input ) const {
    // Make sure to never input empty data.
    if ( input.size() == 0 ) return store_e::DATA_EMPTY;

    // Use Windows to create the directory structure if it does not
    // already exist. If we cannot create it, return a status code
    // that the storage simply does not exist.
    if ( !create_directory_tree( m_path, key ) )
      return store_e::STORAGE_NOEXIST;

    const auto file_path = get_abs_file_path( m_path, key );

    std::ofstream stream( file_path, std::ios::out | std::ios::binary );
    if ( !stream.good() ) { return store_e::STORAGE_BAD; }

    stream.write( reinterpret_cast<const char *>( input.data() ), input.size() );

    return store_e::SUCCESS;
  }
}
}
