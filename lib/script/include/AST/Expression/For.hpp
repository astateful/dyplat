#pragma once

#include "../Expression.hpp"

namespace astateful
{
  namespace script
  {
    /// ForExprAST - Expression class for for/in.
    class ASTExpressionFor : public ASTExpression
    {
      //!
      std::string VarName;

      //!
      std::unique_ptr<ASTExpression> Start, End, Step, Body;
    public:
      //!
      ASTExpressionFor( const std::string&,
                        std::unique_ptr<ASTExpression>&,
                        std::unique_ptr<ASTExpression>&,
                        std::unique_ptr<ASTExpression>&,
                        std::unique_ptr<ASTExpression>&,
                        Context& );

      //!
      llvm::Value *Codegen() override;
    };
  };
};
