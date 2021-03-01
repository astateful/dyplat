#pragma once

#include "../Expression.hpp"

namespace astateful
{
  namespace script
  {
    //! VariableExprAST - Expression class for referencing a variable, like "a".
    class ASTExpressionVariable : public ASTExpression
    {
      //!
      std::string Name;
    public:
      //!
      ASTExpressionVariable( Context& context, const std::string& );

      //!
      const std::string& getName() const { return Name; }

      //!
      llvm::Value * Codegen() override;
    };
  };
};
