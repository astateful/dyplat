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

namespace llvm {
  class LLVMContext;
  class ConstantFolder;
  class AllocaInst;

  template<bool> class IRBuilderDefaultInserter;
  template<bool, typename T, typename Inserter > class IRBuilder;
}

namespace astateful {
namespace script {
  //!
  using builder_t = llvm::IRBuilder<true, llvm::ConstantFolder, llvm::IRBuilderDefaultInserter<true>>;

  //! We create a custom context to hide the LLVM context and not expose LLVM
  //! header files to other libraries.
  struct Context final {
    //! This holds the precedence for each defined binary operator.
    std::map<char, int> m_precedence;

    //!
    std::map<std::string, llvm::AllocaInst *> symbol_table;

    //!
    std::string msOutput = "";

    //!
    int nextToken;

    //!
    //!
    Context();
    ~Context();

    //! 
    //!
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;
    Context( Context&& ) = delete;
    Context& operator=( Context&& ) = delete;

    //!
    //!
    operator llvm::LLVMContext&();

    //!
    //!
    builder_t& builder();

    //!
    //!
    int setNextToken( int next_token );

    //!
    //!
    int tokenPrecedence( int token );
  private:
    //! Use a pimpl to hide the llvm implementation.
    struct pimpl;
    std::unique_ptr<pimpl> m_pimpl;
  };
}
}
