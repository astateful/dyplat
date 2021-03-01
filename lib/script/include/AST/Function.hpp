#pragma once

#include "../AST.hpp"

#include <memory>

namespace astateful {
namespace script {
  //!
  class ASTPrototype;

  //!
  struct ASTExpression;

  //! This class represents a function definition itself.
  struct ASTFunction final : public AST {
    //!
    //!
    ASTFunction( std::unique_ptr<ASTPrototype>&,
                 std::unique_ptr<ASTExpression>&,
                 Context& context );

    //!
    //!
    llvm::Function * Codegen();
  private:
    //!
    std::unique_ptr<ASTPrototype> m_prototype;

    //!
    std::unique_ptr<ASTExpression> m_body;
  };
}
}
