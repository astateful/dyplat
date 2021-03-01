#include "AST/Expression/Binary.hpp"
#include "AST/Expression/Variable.hpp"

#include "astateful/script/JIT.hpp"
#include "astateful/script/Context.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionBinary::ASTExpressionBinary(
      char op,
      std::unique_ptr<ASTExpression>& lhs,
      std::unique_ptr<ASTExpression>& rhs,
      JIT& cJIT,
      Context& context ) :
      ASTExpression( context ),
      Op( op ),
      LHS( std::move( lhs ) ),
      RHS( std::move( rhs ) ),
      mcJIT( cJIT ) {}

    llvm::Value * ASTExpressionBinary::Codegen()
    {
      // Special case '=' because we don't want to emit the LHS as an expression.
      if ( Op == '=' )
      {
        // Assignment requires the LHS to be an identifier.
        ASTExpressionVariable *LHSE = reinterpret_cast<ASTExpressionVariable*>( LHS.get() );
        if ( !LHSE )
        {
          return ErrorV( "destination of '=' must be a variable" );
        }
        // Codegen the RHS.
        llvm::Value *Val = RHS->Codegen();
        if ( Val == 0 ) { return 0; }

        // Look up the name.
        llvm::Value * Variable = m_context.symbol_table[LHSE->getName()];
        if ( Variable == 0 ) { return ErrorV( "Unknown variable name" ); }

        m_context.builder().CreateStore( Val, Variable );

        return Val;
      }

      llvm::Value *L = LHS->Codegen();
      llvm::Value *R = RHS->Codegen();
      if ( L == 0 || R == 0 ) { return 0; }

      switch ( Op )
      {
      case '+': return m_context.builder().CreateFAdd( L, R, "addtmp" );
      case '-': return m_context.builder().CreateFSub( L, R, "subtmp" );
      case '*': return m_context.builder().CreateFMul( L, R, "multmp" );
      case '<':
        L = m_context.builder().CreateFCmpULT( L, R, "cmptmp" );
        // Convert bool 0/1 to double 0.0 or 1.0
        return m_context.builder().CreateUIToFP(
                 L,
                 llvm::Type::getDoubleTy( m_context ),
                 "booltmp"
               );
      default: break;
      }

      // If it wasn't a builtin binary operator, it must be a user defined one. Emit
      // a call to it.
      llvm::Function * F = mcJIT.getFunction( MakeLegalFunctionName( std::string( "binary" ) + Op ) );

      assert( F && "binary operator not found!" );

      llvm::Value * Ops[] = { L, R };

      return m_context.builder().CreateCall( F, Ops, "binop" );
    }
  }
}
