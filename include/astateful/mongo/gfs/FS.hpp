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

#include <string>
#include <memory>

namespace astateful {
namespace bson {
  struct Serialize;
  enum class error_e;
}

namespace mongo {
  struct Context;

namespace gfs {
  using namespace astateful::bson;

  //!
  struct File;

  //!
  struct FS final {
    //!
    //!
    FS( const Context&, const std::string&, const std::string&, error_e& );

    //!
    //!
    FS( const FS& ) = delete;
    FS& operator=( const FS& ) = delete;
    FS& operator=( FS&& ) = delete;
    FS( FS&& ) = delete;

    //! The context used for communicating with the mongo server.
    const Context& context;

    //! The root database name
    const std::string db;

    //!
    const std::string prefix;

    //! The namespace where the file metadata is stored.
    const std::string fileNs;

    //! The namespace where the actual files data is stored.
    const std::string chunkNs;

    //!
    bson::error_e& error;
  };

  //!
  //!
  bool upload( const FS&, File&, const std::string& );

  //!
  //!
  bool download( const FS&, File&, const std::string& );

  //!
  //!
  std::unique_ptr<File> find( FS&, const std::string& );

  //!
  //!
  bool remove( const FS&, const std::string& );
}
}
}
