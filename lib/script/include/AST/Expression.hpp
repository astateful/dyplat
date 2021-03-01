#pragma once

#include "../AST.hpp"

#include <memory>

namespace llvm {
  class Value;
}

namespace astateful {
namespace script {
  //!
  struct ASTExpression : public AST {
    //!
    //!
    ASTExpression( Context& context );

    //!
    //!
    virtual llvm::Value *Codegen() = 0;
  };

  extern std::unique_ptr<ASTExpression> Error( const char * );

  extern llvm::Value * ErrorV( const char * );
}
}
