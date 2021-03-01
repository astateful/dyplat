#pragma once

#include "../Expression.hpp"

#include <vector>

namespace astateful
{
  namespace script
  {
    struct JIT;

    /// CallExprAST - Expression class for function calls.
    class ASTExpressionCall : public ASTExpression
    {
      //!
      std::string Callee;

      //!
      std::vector<std::unique_ptr<ASTExpression>> Args;

      //!
      JIT& mcJIT;
    public:
      //!
      ASTExpressionCall( const std::string&,
                         std::vector<std::unique_ptr<ASTExpression>>&,
                         JIT&,
                         Context& );

      //!
      llvm::Value * Codegen() override;
    };
  };
};
