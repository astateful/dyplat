#pragma once

#include <string>

namespace llvm
{
  class AllocaInst;
  class Function;
}

namespace astateful {
namespace script {
  //!
  struct Context;

  struct AST {
    //!
    //!
    AST( Context& context );
    virtual ~AST() = default;
  protected:
    //!
    Context& m_context;

    //!
    //!
    llvm::AllocaInst * CreateEntryBlockAlloca( llvm::Function *, const std::string& );

    //!
    //!
    std::string MakeLegalFunctionName( std::string );

    //!
    //!
    std::string GenerateUniqueName( const char * );
  };
}
}
