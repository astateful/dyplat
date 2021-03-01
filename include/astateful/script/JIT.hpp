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

#include <memory>
#include <map>
#include <vector>

namespace llvm {
  class raw_fd_ostream;
  class Module;
  class MemoryBufferRef;
  class ExecutionEngine;
  class Function;
  class MemoryBuffer;
  class raw_ostream;
};

namespace astateful {
namespace script {
  //! Forward declare the optional JIT object cache.
  struct Cache;
  struct Context;

  struct JIT final {
    //! Construct a JIT using no IR and without any object cache. This is the
    //! "bare bones implementation and the JIT must be populated manually.
    //!
    //! @param context The LLVM context wrapper.
    //!
    JIT( Context& context );

    //! Construct a JIT with the context and the cache path, which indicates
    //! that the object cache will be used. The JIT must then be populated
    //! manually.
    //!
    //! @param context The LLVM context wrapper.
    //! @param cache_path The absolute path to the object cache folder.
    //!
    JIT( Context& context, const std::string& cache_path );

    //! Construct a JIT with the context, the cache path which indicates that
    //! the object cache will be used, as well as the buffer containing the IR
    //! source to parse into the JIT.
    //!
    //! @param context The LLVM context wrapper.
    //! @param cache_path The absolute path to the object cache folder.
    //! @param buffer A memory buffer pointing to the IR source.
    //!
    JIT( Context& context, const std::string& cache_path,
         const llvm::MemoryBuffer& buffer );

    //! Construct the JIT with the context and the buffer containing the IR
    //! to parse into the JIT. The object cache will not be used.
    //!
    //! @param context The LLVM context wrapper.
    //! @param buffer A memory buffer pointing to the IR source.
    //!
    JIT( Context& context, const llvm::MemoryBuffer& buffer );

    JIT(const JIT& ) = delete;
    JIT& operator=(const JIT&) = delete;
    JIT& operator=( JIT&& ) = delete;

    //!
    //!
    JIT( JIT&& );

    //!
    //!
    llvm::Function * getFunction( const std::string );

    //!
    //!
    llvm::Module * getModuleForNewFunction();

    //!
    //!
    void * getPointerToFunction( llvm::Function * );

    //!
    //!
    void * getPointerToNamedFunction( const std::string& );

    //!
    //!
    void closeCurrentModule();

    //!
    //!
    bool output( llvm::raw_ostream& ) const;

    //!
    //!
    bool ModuleName( std::string& ) const;

    //!
    //!
    ~JIT();
  private:
    //!
    std::unique_ptr<Cache> m_cache;

    //!
    Context& m_context;

    //!
    std::vector<std::unique_ptr<llvm::Module>> module_list = {};

    //!
    std::map<llvm::Module *, llvm::ExecutionEngine *> m_engine = {};

    //!
    llvm::Module * current_module = nullptr;

    //!
    //!
    llvm::ExecutionEngine * compileModule( std::unique_ptr<llvm::Module>& );

    //!
    //!
    std::string GenerateUniqueName( const std::string& );
  };

  //! Given a context, the filename, the cache path and the source path,
  //! try to construct a valid JIT. The JIT is constructed based on the
  //! existence of an IR file corresponding to the filename. If IR does not
  //! exist, it will be created from the source and saved to cache path. If
  //! it does exist in the cache path, the JIT will use that.
  //!
  JIT generate_jit( Context& context,
                    const std::string& file_name,
                    const std::string& cache_path,
                    const std::string& source_path );
  }
}
