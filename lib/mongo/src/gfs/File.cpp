#include "astateful/mongo/gfs/File.hpp"
#include "astateful/mongo/gfs/FS.hpp"
#include "astateful/mongo/Iterate.hpp"
#include "astateful/mongo/Context.hpp"
#include "astateful/bson/Iterate.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/bson/Element/Long.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Datetime.hpp"
#include "astateful/bson/Element/Binary/Generic.hpp"
#include "astateful/bson/Error.hpp"

#include <cassert>
#include <iostream>

namespace astateful {
namespace mongo {
namespace gfs {
  File::File( FS& fs, const std::string& content_type, const std::string& name ) :
    m_id(),
    m_content_type( content_type ),
    m_name( name ),
    m_md5(),
    m_chunk_size( 256 * 1024 ),
    m_fs( fs ),
    m_pending(),
    m_num_chunks( 0 ),
    m_length( 0 ) {
    assert( m_name.size() > 0 );
    assert( m_content_type.size() > 0 );

    Serialize data;
    data.append( ElementObjectId( "_id", m_id ) );
    data.append( ElementInt( "chunkSize", m_chunk_size ) );
    data.append( ElementString( "filename", m_name ) );
    data.append( ElementDatetime( "uploadDate" ) );
    data.append( ElementString( "contentType", m_content_type ) );

    Serialize condition;
    condition.append( ElementObjectId( "_id", m_id ) );

    assert( update( m_fs.context, m_fs.fileNs, condition, data, update_e::UPSERT,
                    m_fs.error ) );
  }

  File::File( FS& fs, const Serialize& meta ) :
    m_num_chunks( 0 ),
    m_pending(),
    m_id( *meta.at( "_id" ) ),
    m_content_type( *meta.at( "contentType" ) ),
    m_name( *meta.at( "filename" ) ),
    m_md5( *meta.at( "md5" ) ),
    m_chunk_size( *meta.at( "chunkSize" ) ),
    m_fs( fs ),
    m_length( int64_t( *meta.at( "length" ) ) ) {
    // Find the number of chunks stored on the server.
    {
      Serialize result;
      Serialize query;
      query.append( ElementObjectId( "files_id", m_id ) );

      assert( count( fs.context, fs.chunkNs, m_fs.error, result, &query ) );
      assert( double( result["ok"] ) == 1.000000 );

      m_num_chunks = result["n"];
    }

    // If we have a remainder, then the last chunk is not complete! This is
    // really stupid what we do here: remove the chunk from the database and
    // make it our pending buffer. The reasoning is that we need to somehow
    // update this pending data, and rather than be smart, we are dumb for now.
    // This means we have to somehow lock the file!
    if ( m_length % m_chunk_size > 0 ) {
      // Load the pending chunk into the file.
      {
        Serialize result;
        Serialize query;
        query.append( ElementObjectId( "files_id", m_id ) );
        query.append( ElementInt( "n", m_num_chunks - 1 ) );
        assert( find_one( fs.context, fs.chunkNs, query, result, m_fs.error ) );

        m_pending = result["data"];
      }

      // Remove the incomplete chunk from the server.
      {
        Serialize query;
        query.append( ElementObjectId( "files_id", m_id ) );

        Serialize n;
        n.append( ElementInt( "$eq", m_num_chunks - 1 ) );

        assert( query.append( "n", n, m_fs.error ) );
        assert( remove( m_fs.context, m_fs.chunkNs, query, m_fs.error ) );

        --m_num_chunks;
      }
    }
  }

  bool File::flush() {
    Serialize condition;
    condition.append( ElementObjectId( "files_id", m_id ) );
    condition.append( ElementInt( "n", m_num_chunks ) );
    condition.append( ElementBinaryGeneric( "data", m_pending ) );

    Serialize query;
    query.append( ElementInt( "n", m_num_chunks ) );
    query.append( ElementObjectId( "files_id", m_id ) );

    return update( m_fs.context, m_fs.chunkNs, query, condition, update_e::UPSERT,
                   m_fs.error );
  }

  uint64_t File::writeBuffer( const std::vector<uint8_t>& buffer ) {
    int64_t to_process = buffer.size();
    int64_t written = 0;

    // See if we have a partial chunk.
    size_t remainder = m_length % m_chunk_size;

    if ( remainder ) {
      auto pending_size = m_pending.size();

      if ( pending_size + buffer.size() <= m_chunk_size ) {
        // Insert the buffer directly into the pending data since it will
        // be less than the chunk size.
        m_pending.insert( m_pending.end(), buffer.begin(), buffer.end() );

        to_process -= buffer.size();
        m_length += buffer.size();
        written += buffer.size();
      } else {
        auto overflow = pending_size + buffer.size() - m_chunk_size;

        // Subtract the overflow bytes from the buffer that would make
        // the pending data size greater than the chunk size.
        m_pending.insert( m_pending.end(),
                          buffer.begin(),
                          buffer.end() - overflow );

        // 
        auto x = std::distance( buffer.begin(), buffer.end() - overflow );

        if ( !flush() ) return buffer.size() - x;

        to_process -= x;
        m_length += x;
        written += x;

        m_num_chunks++;
        m_pending.clear();
      }
    }

    while ( to_process >= m_chunk_size ) {
      // 
      assert( m_pending.size() == 0 );

      m_pending.insert( m_pending.begin(),
                        buffer.begin() + written,
                        buffer.begin() + written + m_chunk_size );

      // If we could not upsert this chunk then we have only written
      // the size of the buffer minus whatever is left over.
      if ( !flush() ) return buffer.size() - to_process;

      to_process -= m_chunk_size;
      m_length += m_chunk_size;
      written += m_chunk_size;

      ++m_num_chunks;
      m_pending.clear();
    }

    if ( to_process > 0 ) {
      m_pending.insert( m_pending.end(),
                        buffer.begin() + written,
                        buffer.begin() + written + to_process );

      m_length += to_process;
      written += to_process;
      to_process -= to_process;
    }

    assert( written == buffer.size() );
    assert( to_process == 0 );

    return written;
  }

  uint64_t File::readBuffer( std::vector<uint8_t>& buffer,
                             uint64_t offset ) const {
    if ( offset >= m_length ) return 0;

    int read = 0;
    int first_chunk = 0;
    auto to_process = buffer.size();
    auto relative_offset = offset;

    // Retrieve the first chunk. An offset means that initial chunks can be
    // completely ignored if the offset begins past their boundary.
    auto y = offset % m_chunk_size;
    if ( y > 0 ) {
      first_chunk = static_cast<int>( ( offset - y ) / m_chunk_size );
    } else {
      first_chunk = static_cast<int>( offset / m_chunk_size );
    }

    relative_offset -= ( m_chunk_size * first_chunk );

    // Retrieve the last chunk. An offset means that ending chunks can be
    // completely discarded if the offset ends before they begin.
    auto buffer_end_offset = offset + buffer.size();
    auto last_chunk = static_cast<int>( std::floor( buffer_end_offset / m_chunk_size ) + 1 );

    Serialize query;
    query.append( ElementObjectId( "files_id", m_id ) );

    Serialize n;
    n.append( ElementInt( "$gte", first_chunk ) );
    assert( query.append( "n", n, m_fs.error ) );

    Serialize orderby;
    orderby.append( ElementInt( "n", 1 ) );

    Serialize command;
    assert( command.append( "query", query, m_fs.error ) );
    assert( command.append( "orderby", orderby, m_fs.error ) );

    Iterate iterate( m_fs.context, m_fs.chunkNs, query, last_chunk, 0, 0, m_fs.error );

    bool sentinel = false;
    int current_chunk = 0;

    while ( iterate ) {
      std::vector<uint8_t> data = *iterate->at( "data" );

      size_t chunk_offset = ( current_chunk == 0 ) ? relative_offset : 0;

      for ( auto i = chunk_offset; i < data.size(); ++i ) {
        if ( read < buffer.size() ) {
          buffer[read] = data[i];
          ++read;
          --to_process;
        } else {
          sentinel = true;
          break;
        }
      }

      if ( sentinel ) break;

      ++iterate;
      ++current_chunk;
    }

    if ( to_process > 0 ) {
      for ( const auto& i : m_pending ) {
        if ( read < buffer.size() ) {
          buffer[read] = i;
          ++read;
          --to_process;
        }
      }
    }

    assert( read == buffer.size() );
    assert( to_process == 0 );

    return read;
  }

  File::~File() {
    bool flushed = true;

    // Only flush if there is something to flush. This would not be the case
    // if the files data size was a multiple of the chunk size.
    if ( m_pending.size() > 0 ) flushed = flush();

    // If the flush was not successful, do not write the file
    // since it would be corrupt.
    if ( flushed ) {
      ++m_num_chunks;

      Serialize md5;
      assert( md5_lookup( m_fs.context, m_fs.db, m_id, m_fs.prefix, md5, m_fs.error ) );

      Serialize field;
      field.append( ElementLong( "length", m_length ) );
      field.append( ElementInt( "chunkSize", m_chunk_size ) );
      field.append( ElementString( "md5", std::string( md5["md5"] ) ) );

      Serialize condition;
      condition.append( ElementObjectId( "_id", m_id ) );

      Serialize data;
      assert( data.append( "$set", field, m_fs.error ) );

      assert( update( m_fs.context, m_fs.fileNs, condition, data, update_e::UPSERT,
                     m_fs.error ) );
    }
  }
}
}
}
