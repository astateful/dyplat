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

#include <llvm/ExecutionEngine/ObjectCache.h>

namespace astateful {
namespace script {
  //! The purpose of this class is to avoid compiling already compiled modules
  //! for which an object file is available. Note that this class deals with
  //! Bitcode files and NOT IR files.
  struct Cache final : public llvm::ObjectCache {
    //! Provide the object cache with the name of the caching directory. All
    //! file lookups will happen within this directory.
    //!
    explicit Cache( const std::string& );

    //! This method will be triggered once the JIT has compiled a module. The
    //! id of the module then determines whetherit should then be cached or not.
    //!
    void notifyObjectCompiled( const llvm::Module *, llvm::MemoryBufferRef ) override;

    //! The JIT will call this method before compiling any module. If a module exists
    //! then all this method needs to do is return the compiled IR.
    //!
    std::unique_ptr<llvm::MemoryBuffer> getObject( const llvm::Module * ) override;
  private:
    //! The path to the cache directory where compiled modules are stored.
    const std::string m_path;
  };
}
}
