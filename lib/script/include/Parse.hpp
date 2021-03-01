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

namespace astateful {
namespace script {
  //!
  struct Context;

  //!
  struct Lexer;

  //!
  struct JIT;

  //! The parser should remain completely stateless.
  struct Parse final {
    //! The parser enters a loop in which tokens are extracted from the lexer
    //! and parsed according to the rules of the grammar. As the AST is formed
    //! the JIT is populated and produces in the end a final output that is
    //! stored within the context.
    //!
    //! @param jit The JIT is used to dynamically execute parsed functions.
    //! @param context Contains all context sensitive information.
    //! @param lexer Abstracts the buffer containing the tokens to parse.
    //!
    void operator()( JIT& jit, Context& context, Lexer& lexer );
  };
}
}
