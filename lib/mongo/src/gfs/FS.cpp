#include "astateful/mongo/gfs/FS.hpp"
#include "astateful/mongo/gfs/File.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/mongo/Iterate.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Long.hpp"
#include "astateful/bson/Element/Datetime.hpp"
#include "astateful/bson/ObjectId.hpp"

#include <cassert>
#include <iostream>
#include <fstream>

namespace astateful {
namespace mongo {
namespace gfs {
  FS::FS( const mongo::Context& context_,
          const std::string& db_,
          const std::string& prefix_,
          error_e& error_ ) :
    context( context_ ),
    db( db_ ),
    prefix( prefix_ ),
    error( error_ ),
    fileNs( db + "." + prefix + ".files" ),
    chunkNs( db + "." + prefix + ".chunks" ) {
    Serialize empty;

    Serialize file;
    file.append( ElementInt( "filename", 1 ) );

    assert( create_index( context, fileNs, file, "default_files_index", 0, -1,
      empty, error ) );

    Serialize chunk;
    chunk.append( ElementInt( "files_id", 1 ) );
    chunk.append( ElementInt( "n", 1 ) );

    assert( create_index( context, chunkNs, chunk, "default_chunk_index",
      static_cast<int>(index_e::UNIQUE), -1, empty, error ) );
  }

  bool upload( const FS& fs, File& file, const std::string& file_name ) {
    std::ifstream stream( file_name, std::ios::in | std::ios::binary );
    if ( !stream.good() ) return false;

    // Need the size of the stream to determine if entire stream was written.
    stream.seekg( 0, std::ios::end );
    auto file_size = static_cast<uint64_t>( stream.tellg() );
    stream.seekg( 0, std::ios::beg );

    std::vector<uint8_t> buffer( file.chunkSize() );
    uint64_t bytes_written = 0;

    // Declare this lambda for brevity; need to do same op twice.
    auto write_buffer = [ & ]() -> uint64_t {
      if ( buffer.size() != stream.gcount() ) buffer.resize( stream.gcount() );
      return file.writeBuffer( buffer );
    };

    while ( stream.read( reinterpret_cast<char*>( &buffer[0] ), buffer.size() ) )
      bytes_written += write_buffer();

    // The loop has terminated probably because the end of file has been
    // reached.However data was still read and remains in the buffer so be sure
    // to write the remaining data as well.
    if (stream.gcount() > 0) bytes_written += write_buffer();

    return ( bytes_written == file_size );
  }

  bool download( const FS& fs, File& file, const std::string& file_name ) {
    // Remember to open in append mode since we read chunk by chunk.
    std::ofstream stream( file_name, std::ios::out | std::ios::app
                          | std::ios::binary );
    if ( !stream.good() ) return false;

    std::vector<uint8_t> buffer( file.chunkSize() );
    uint64_t bytes_read = 0, read = 0;

    while ( ( read = file.readBuffer( buffer, bytes_read ) ) > 0 ) {
      // There is no actual way to get the number of bytes written
      // due to OS flushing and buffering and delayed writing.
      stream.write( reinterpret_cast<char*>( &buffer[0] ), read );
      bytes_read += read;
    }

    return ( bytes_read == file.contentLength() );
  }

  std::unique_ptr<File> find( FS& fs, const std::string& file_name ) {
    Serialize query;
    query.append( ElementString( "filename", file_name ) );

    Serialize upload_date;
    upload_date.append( ElementInt( "uploadDate", -1 ) );

    Serialize command;
    if ( !command.append( "query", query, fs.error ) ) return nullptr;
    if ( !command.append( "orderby", upload_date, fs.error ) ) return nullptr;

    Serialize result;
    if ( !find_one( fs.context, fs.fileNs, command, result, fs.error ) )
      return nullptr;

    return std::make_unique<File>( fs, result );
  }

  bool remove( const FS& fs, const std::string& file_name ) {
    bool result = false;
    Serialize query;
    query.append( ElementString( "filename", file_name ) );

    Iterate files( fs.context, fs.fileNs, query, 0, 0, 0, fs.error );

    while ( files )
    {
      ObjectId id = ( *files )["_id"];

      Serialize file;
      file.append( ElementObjectId( "_id", id ) );

      mongo::remove( fs.context, fs.fileNs, file, fs.error );

      Serialize chunk;
      chunk.append( ElementObjectId( "files_id", id ) );

      result = mongo::remove( fs.context, fs.chunkNs, chunk, fs.error );

      ++files;
    }

    return result;
  }
}
}
}
