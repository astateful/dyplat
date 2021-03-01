#define _WINSOCKAPI_
#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/mongo/gfs/File.hpp"
#include "astateful/mongo/gfs/FS.hpp"
#include "astateful/mongo/Iterate.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/async/net/client/Engine.hpp"
#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/server/Engine.hpp"
#include "astateful/crypto/md5.hpp"

#include <fstream>
#include <mutex>
#include <random>

using namespace astateful;
using namespace astateful::async;
using namespace astateful::bson;
using namespace astateful::mongo;
using namespace astateful::mongo::gfs;

namespace {
  std::vector<uint8_t> create_random_buffer( const size_t size ) {
    std::vector<uint8_t> buffer( size );
    const std::string pool( "abcdefghijklmnopqrstuvwxyz" );

    std::random_device rd;
    std::mt19937 rng( rd() );
    std::uniform_int_distribution<int> uni( 0, static_cast<int>( pool.size() ) );

    for ( auto& ch : buffer ) {
      ch = pool[uni( rng )];
    }

    return buffer;
  }

  std::vector<std::vector<uint8_t>> buffer_set;
}

int main( int argc, char* const argv[] ) {
  std::mutex mutex;
  std::unique_lock<std::mutex> lock( mutex, std::defer_lock );

  pipe::server::callback_t callback;
  callback["log"] = [&]( const std::vector<uint8_t>& buffer ) {
    lock.lock();
    //std::string data( buffer.begin()+1, buffer.end() );
    lock.unlock();
  };

  pipe::server::Engine pipe( callback );
  pipe.start();

  // Since it is unknown to this author how to integrate the vector into
  // the tests without having it always repopulate, create it once here.
  buffer_set.push_back( create_random_buffer( 1024 + ( 256 * 1024 ) ) );
  buffer_set.push_back( create_random_buffer( ( 256 * 1024 ) - 1024 + 1 ) );
  buffer_set.push_back( create_random_buffer( 1024 ) );
  buffer_set.push_back( create_random_buffer( ( 256 * 1024 ) * 3 ) );

  int result = Catch::Session().run( argc, argv );

  pipe.kill();

  return result;
}

TEST_CASE( "mongo" ) {
  const std::wstring address = L"localhost:27017";
  Serialize empty;

  net::client::connection_t connection;
  connection[address] = [&]( net::client::Response * response,
                             const net::client::Request& request ) {
    return new net::client::Connection( response, request );
  };

  pipe::client::Engine pipe;
  pipe.start();

  net::client::Engine engine( pipe, connection );
  REQUIRE( engine.runIPv4() );

  auto error = error_e::CLEAN;

  Context context( engine, address, error );
  FS fs( context, "test", "fs", error );

  SECTION( "get master status" ) {
    Serialize result;
    REQUIRE( get_master_status( context, result, error ) );
    REQUIRE( int32_t( result["maxBsonObjectSize"] ) == context.maxBsonSize );
    REQUIRE( double( result["ok"] ) == 1.000000 );
  }

  SECTION( "drop database" ) {
    Serialize result;
    REQUIRE( drop_database( context, "test", result, error ) );
    REQUIRE( double( result["ok"] ) == 1.000000 );
    REQUIRE( std::string( result["dropped"] ) == "test" );
  }

  SECTION( "find" ) {
    Serialize result;
    REQUIRE( !find_one( context, "test.c.simple", empty, result, error ) );
  }

  SECTION( "insert" ) {
    for ( int i = 0; i < 200; i++ ) {
      Serialize document;
      document.append( ElementInt( "key", i ) );
      REQUIRE( insert( context, "test.c.simple", document, error ) );
    }
  }

  SECTION( "insert batch" ) {
    std::vector<Serialize> documents( 50 );
    for ( auto& document : documents  ) {
      document.append( ElementString( "key", "value" ) );
    }

    REQUIRE( insert_batch( context, "test.c.batch", documents, true, error ) );
  }

  SECTION( "count" ) {
    Serialize result;
    REQUIRE( count( context, "test.c.simple", error, result, &empty ) );
    REQUIRE( double( result["ok"] ) == 1.000000 );
    REQUIRE( int32_t( result["n"] ) == 200 );
  }

  SECTION( "ping" ) {
    Serialize result;
    REQUIRE( ping( context, result, error ) );
    REQUIRE( double( result["ok"] ) == 1.000000 );
  }

  REQUIRE( add_user( context, "test", "user_name", "password", error ) );

  SECTION( "create simple index" ) {
    Serialize result;
    REQUIRE( create_simple_index( context,
      "test.c.simple",
      "key",
      "simple_index",
      static_cast<int>(index_e::UNIQUE),
      result,
      error ) );

    REQUIRE( double( result["ok"] ) == 1.000000 );
  }

  SECTION( "create capped collection" ) {
    Serialize result;
    REQUIRE( create_capped_col( context, "test.c.simple_capped", 30, 5, result, error ) );
    REQUIRE( double( result["ok"] ) == 1.000000 );
  }

  SECTION( "reset error" ) {
    Serialize result;
    REQUIRE( reset_error( context, "test", result, error ) );
    REQUIRE( double( result["ok"] ) == 1.000000 );
  }

  SECTION( "iterate" ) {
    Serialize field;
    field.append( ElementInt( "key", 1 ) );

    Serialize query;
    query.append( "$orderby", field, error );
    query.append( "$query", empty, error );

    SECTION( "no limit" ) {
      Iterate iterate( context, "test.c.simple", query, 0, 0, 0, error );

      int32_t counter = 0;
      while ( iterate ) {
        REQUIRE( int32_t( ( *iterate )["key"] ) == counter );

        ++counter;
        ++iterate;
      }
      REQUIRE( counter == 200 );
    }

    SECTION( "skip" ) {
      int counter = 0;
      int offset = 5;

      Iterate iterate( context, "test.c.simple", query, 0, offset, 0, error );
      while ( iterate ) {
        REQUIRE( int32_t( ( *iterate )["key"] ) == offset + counter );

        ++counter;
        ++iterate;
      }
      REQUIRE( counter == 195 );
    }

    SECTION( "limit" ) {
      int counter = 0;
      int limit = 105;

      Iterate iterate( context, "test.c.simple", query, limit, 0, 0, error );
      while ( iterate ) {
        REQUIRE( int32_t( ( *iterate )["key"] ) == counter );

        ++counter;
        ++iterate;
      }
      REQUIRE( counter == limit );
    }

    SECTION( "limit and skip" ) {
      int counter = 0;
      int offset = 13;
      int limit = 105;

      Iterate iterate( context, "test.c.simple", query, limit, offset, 0, error );
      while ( iterate ) {
        REQUIRE( int32_t( ( *iterate )["key"] ) == offset + counter );

        ++counter;
        ++iterate;
      }
      REQUIRE( counter == limit );
    }
  }

  SECTION( "update" ) {
    SECTION( "upsert" ) {
      Serialize data;
      data.append( ElementString( "key", "new" ) );

      Serialize condition;
      condition.append( ElementString( "key", "0" ) );
      REQUIRE( update( context, "test.c.simple", condition, data,
        update_e::UPSERT, error ) );
    }

    SECTION( "remove upserted value" ) {
      Serialize condition;
      condition.append( ElementString( "key", "0" ) );
      REQUIRE( remove( context, "test.c.simple", condition, error ) );
    }
  }

  SECTION( "drop collection" ) {
    Serialize result;
    REQUIRE( drop_collection( context, "test", "c.simple", result, error ) );
    REQUIRE( double( result["ok"] ) == 1.000000 );
    REQUIRE( std::string( result["ns"] ) == "test.c.simple" );
    REQUIRE( int( result["nIndexesWas"] ) == 2 );
  }

  SECTION( "gfs" ) {
    SECTION( "upload & download" ) {
      size_t buffer_size = 256 * 1024 * 3;
      auto ibuffer = create_random_buffer( buffer_size );
      std::vector<uint8_t> obuffer( buffer_size );

      remove( "input-file" );
      remove( "output-file" );

      std::ofstream ostream( "input-file", std::ios::out | std::ios::binary );
      ostream.write( reinterpret_cast<char*>( ibuffer.data() ), ibuffer.size() );
      ostream.close();

      {
        File ufile( fs, "text/html", "input-file" );
        REQUIRE( upload( fs, ufile, "input-file" ) );
      }

      std::unique_ptr<File> dfile = find( fs, "input-file");
      REQUIRE( dfile );
      REQUIRE( download( fs, *dfile, "output-file" ) );

      std::ifstream istream( "output-file", std::ios::in | std::ios::binary );
      istream.read( reinterpret_cast<char*>( &obuffer[0] ), obuffer.size() );
      REQUIRE( ibuffer == obuffer );
    }

    SECTION( "delete" ) {
      SECTION( "create a file and fill it with an empty buffer" ) {
        std::vector<uint8_t> data( 1024 );
        File file( fs, "text/html", "test-delete" );
        REQUIRE( file.writeBuffer( data ) == data.size() );
      }

      SECTION( "find the file on the server and delete it twice" ) {
        auto file = find( fs, "test-delete" );
        REQUIRE( file );
        REQUIRE( remove( fs, "test-delete" ) );
        REQUIRE( !find( fs, "test-delete" ) );
      }

      SECTION( "delete a file which never existed" ) {
        REQUIRE( !find( fs, "bogus-file-does-not-exist" ) );
        REQUIRE( !remove( fs, "bogus-file-does-not-exist" ) );
      }
    }

    SECTION( "stream" ) {
      SECTION( "write to and read from a non-flushed file" ) {
        File file( fs, "text/html", "buffer" );

        uint64_t total_size = 0;
        uint64_t offset = 0;

        for ( const auto& buffer : buffer_set ) {
          total_size += buffer.size();

          REQUIRE( file.writeBuffer( buffer ) == buffer.size() );
          REQUIRE( file.contentLength() == total_size );
        }

        for ( const auto& buffer : buffer_set ) {
          std::vector<uint8_t> read_buffer( buffer.size() );

          REQUIRE( file.readBuffer( read_buffer, offset ) == read_buffer.size() );
          REQUIRE( buffer == read_buffer );

          offset += buffer.size();
        }
      }

      SECTION( "write to and read from a flushed file" ) {
        auto file = find( fs, "buffer" );
        REQUIRE( file );

        buffer_set.push_back( create_random_buffer( ( 256 * 1024 ) * 3 ) );

        uint64_t total_size = file->contentLength();
        uint64_t offset = 0;

        total_size += buffer_set.back().size();

        REQUIRE( file->writeBuffer( buffer_set.back() ) == buffer_set.back().size() );
        REQUIRE( file->contentLength() == total_size );

        for ( const auto& buffer : buffer_set ) {
          std::vector<uint8_t> read_buffer( buffer.size() );

          REQUIRE( file->readBuffer( read_buffer, offset ) == read_buffer.size() );
          REQUIRE( buffer == read_buffer );

          offset += buffer.size();
        }
      }
    }
  }
}
