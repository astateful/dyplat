//! The MIT License (MIT)
//!
//! Copyright (c) 2014 Thomas Kovacs
//!
//! Permission is hereby granted, free of charge, to any person obtaining a
//! copy of this software and associated documentation files (the "Software"),
//! to deal in the Software without restriction, including without limitation
//! the rights to use, copy, modify, merge, publish, distribute, sublicense,
//! and / or sell copies of the Software, and to permit persons to whom the
//! Software is furnished to do so, subject to the following conditions :
//!
//! The above copyright notice and this permission notice shall be included in
//! all copies or substantial portions of the Software.
//!
//! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//! FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//! DEALINGS IN THE SOFTWARE.
#pragma once

#include "astateful/bson/ObjectId.hpp"
#include "astateful/bson/Serialize.hpp"

namespace astateful {
namespace mongo {
namespace gfs {
  using namespace astateful::bson;

  //!
  struct FS;

  //!
  struct File final {
    //! Upon construction of a file, it is also inserted into mongodb, so keep
    //! this in mind when constructing new files!
    //!
    //! @param fs The fs layer containing the database context.
    //! @param content_type The content type of the file binary data.
    //! @param name The real name of the file to create.
    //!
    File( FS& fs, const std::string& content_type, const std::string& name );

    //! Construct a file from data which already exists in the mongodb server.
    //!
    //! @param fs The fs layer containing the database context.
    //! @param meta The metadata containing the real file information.
    //!
    File( FS& fs, const Serialize& meta );

    //!
    File( const File& ) = delete;
    File& operator=(const File& ) = delete;

    //!
    //!
    uint64_t writeBuffer( const std::vector<uint8_t>& buffer );

    //!
    //!
    uint64_t readBuffer( std::vector<uint8_t>& buffer, uint64_t offset ) const;

    //!
    //!
    uint64_t contentLength() const { return m_length; }

    //!
    //!
    const std::string& md5() const { return m_md5; }

    //!
    //!
    const std::string& contentType() const { return m_content_type; }

    //!
    //!
    const std::string& name() const { return m_name; }

    //!
    //!
    const ObjectId& id() const { return m_id; }

    //!
    //!
    int chunkSize() const { return m_chunk_size; }

    //! When the file is destroyed, the pending data buffer will be written to
    //! the FS as an incomplete chunk.
    //!
    ~File();
  private:
    //! The files_id of the gridfile.
    const ObjectId m_id;

    //! The gridfile's content type.
    const std::string m_content_type;

    //! The name of the gridfile as a string.
    const std::string m_name;

    //!
    const std::string m_md5;

    //! Cache the chunk size to avoid a metadata lookup.
    const int32_t m_chunk_size;

    //!
    FS& m_fs;

    //! The pending buffer stores an incomplete chunk.
    std::vector<uint8_t> m_pending;

    //! The number of discrete units that comprise this file. Note that this
    //! number is the number of chunks that reside on the server.
    int m_num_chunks;

    //! The entire size of the file which is basically the total length of all
    //! chunks pertaining to this file.
    uint64_t m_length;

    //!
    //!
    bool flush();
  };
}
}
}
