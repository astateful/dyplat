#pragma once

#include "../Expression.hpp"

namespace astateful
{
  namespace script
  {
    /// IfExprAST - Expression class for if/then/else.
    class ASTExpressionIf : public ASTExpression
    {
      //!
      std::unique_ptr<ASTExpression> Cond, Then, Else;
    public:
      //!
      //!
      ASTExpressionIf( std::unique_ptr<ASTExpression>&,
                       std::unique_ptr<ASTExpression>&,
                       std::unique_ptr<ASTExpression>&,
                       Context& );

      //!
      //!
      llvm::Value * Codegen() override;
    };
  };
};
