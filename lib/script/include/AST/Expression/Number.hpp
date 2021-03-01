#pragma once

#include "../Expression.hpp"

namespace astateful {
namespace script {
  //! Expression class for numeric literals like "1.0".
  struct ASTExpressionNumber : public ASTExpression {
    //!
    ASTExpressionNumber( double, Context& );

    //!
    llvm::Value * Codegen() override;
  private:
    //!
    double m_value;
  };
}
}
