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

#include <llvm/ExecutionEngine/SectionMemoryManager.h>

namespace astateful {
namespace script {
  struct JIT;

  //! Supports allocating memory for section based loading of objects in order to
  //! support the MC JIT.
  struct Memory final : public llvm::SectionMemoryManager {
    //!
    //!
    Memory( JIT& jit );

    //!
    //!
    Memory( const Memory& ) = delete;
    Memory& operator=( const Memory& ) = delete;

    /// This method returns the address of the specified function.
    /// Our implementation will attempt to find functions in other
    /// modules associated with the MCJITHelper to cross link functions
    /// from one generated module to another.
    ///
    /// If \p AbortOnFailure is false and no function with the given name is
    /// found, this function returns a null pointer. Otherwise, it prints a
    /// message to stderr and aborts.
    void * getPointerToNamedFunction( const std::string&, bool AbortOnFailure = true ) override;

    //!
    //!
    uint64_t getSymbolAddress( const std::string& Name ) override;
  private:
    //!
    JIT& m_jit;
  };
}
}
