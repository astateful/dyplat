#pragma once

#include "../Expression.hpp"

namespace astateful
{
  namespace script
  {
    struct JIT;

    /// UnaryExprAST - Expression class for a unary operator.
    class ASTExpressionUnary : public ASTExpression
    {
      //!
      char Opcode;

      //!
      std::unique_ptr<ASTExpression> Operand;

      //!
      JIT& mcJIT;
    public:
      //!
      ASTExpressionUnary( char, std::unique_ptr<ASTExpression>&, JIT&, Context& );

      //!
      llvm::Value * Codegen() override;
    };
  };
};
