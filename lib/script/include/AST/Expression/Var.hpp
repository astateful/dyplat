#pragma once

#include "../Expression.hpp"

#include <vector>
#include <map>
#include <memory>

namespace astateful
{
  namespace script
  {
    /// VarExprAST - Expression class for var/in
    class ASTExpressionVar : public ASTExpression
    {
      //!
      std::vector<std::pair <std::string, std::unique_ptr<ASTExpression>>> VarNames;

      //!
      std::unique_ptr<ASTExpression> Body;
    public:
      //!
      ASTExpressionVar (
        std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>>>&,
        std::unique_ptr<ASTExpression>&,
        Context& context
      );

      //!
      llvm::Value * Codegen() override;
    };
  };
};
