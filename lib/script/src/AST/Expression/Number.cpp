#include "AST/Expression/Number.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/Constants.h>

namespace astateful {
namespace script {
  ASTExpressionNumber::ASTExpressionNumber( double value, Context& context ) :
    ASTExpression( context ),
    m_value( value ) {}

  llvm::Value * ASTExpressionNumber::Codegen() {
    return llvm::ConstantFP::get( m_context, llvm::APFloat( m_value ) );
  }
}
}
