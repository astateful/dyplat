#pragma once

#include "../Expression.hpp"

namespace astateful
{
  namespace script
  {
    struct JIT;

    /// BinaryExprAST - Expression class for a binary operator.
    class ASTExpressionBinary : public ASTExpression
    {
      //!
      char Op;

      //!
      std::unique_ptr<ASTExpression> LHS, RHS;

      //!
      JIT& mcJIT;
    public:
      //!
      ASTExpressionBinary(
        char,
        std::unique_ptr<ASTExpression>&,
        std::unique_ptr<ASTExpression>&,
        JIT&,
        Context& );

      //!
      llvm::Value * Codegen() override;
    };
  };
};