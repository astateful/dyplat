#include "AST/Expression/Variable.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/IRBuilder.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionVariable::ASTExpressionVariable(
      Context& context,
      const std::string& name ) :
      ASTExpression( context ),
      Name( name ) {}

    llvm::Value * ASTExpressionVariable::Codegen ()
    {
      // Look this variable up in the function.
      auto V = m_context.symbol_table[Name];

      if ( V == 0 ) return ErrorV( "Unknown variable name" );

      // Load the value.
      return m_context.builder().CreateLoad( V, Name.c_str() );
    }
  }
}
