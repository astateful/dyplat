#include "AST/Expression/Unary.hpp"

#include "astateful/script/JIT.hpp"
#include "astateful/script/Context.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionUnary::ASTExpressionUnary(
      char opcode,
      std::unique_ptr<ASTExpression>& operand,
      JIT& cJIT,
      Context& context ) :
      ASTExpression( context ),
      Opcode( opcode ),
      Operand( std::move( operand ) ),
      mcJIT( cJIT ) {}

    llvm::Value * ASTExpressionUnary::Codegen()
    {
      auto OperandV = Operand->Codegen();
      if ( OperandV == 0 ) { return 0; }
      auto F = mcJIT.getFunction( MakeLegalFunctionName( std::string( "unary" ) + Opcode ) );

      if ( F == 0 ) return ErrorV( "Unknown unary operator" );

      return m_context.builder().CreateCall( F, OperandV, "unop" );
    }
  }
}
