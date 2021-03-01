#include "AST/Expression/If.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionIf::ASTExpressionIf (
      std::unique_ptr<ASTExpression>& cond,
      std::unique_ptr<ASTExpression>& then,
      std::unique_ptr<ASTExpression>& _else,
      Context& context ) :
      ASTExpression( context ),
      Cond( std::move( cond ) ),
      Then( std::move( then ) ),
      Else( std::move( _else ) ) {}

    llvm::Value * ASTExpressionIf::Codegen ()
    {
      auto CondV = Cond->Codegen();
      if ( CondV == 0 ) { return 0; }

      // Convert condition to a bool by comparing equal to 0.0.
      CondV = m_context.builder().CreateFCmpONE (
                CondV,
                llvm::ConstantFP::get( m_context, llvm::APFloat( 0.0 ) ),
                "ifcond"
              );

      auto TheFunction = m_context.builder().GetInsertBlock()->getParent();

      // Create blocks for the then and else cases.  Insert the 'then' block at the
      // end of the function.
      auto ThenBB = llvm::BasicBlock::Create( m_context, "then", TheFunction );
      auto ElseBB = llvm::BasicBlock::Create( m_context, "else" );
      auto MergeBB = llvm::BasicBlock::Create( m_context, "ifcont" );

      m_context.builder().CreateCondBr( CondV, ThenBB, ElseBB );

      // Emit then value.
      m_context.builder().SetInsertPoint( ThenBB );

      auto ThenV = Then->Codegen();
      if ( ThenV == 0 ) { return 0; }

      m_context.builder().CreateBr( MergeBB );

      // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
      ThenBB = m_context.builder().GetInsertBlock();

      // Emit else block.
      TheFunction->getBasicBlockList().push_back( ElseBB );
      m_context.builder().SetInsertPoint( ElseBB );

      auto ElseV = Else->Codegen();
      if ( ElseV == 0 ) { return 0; }

      m_context.builder().CreateBr( MergeBB );
      // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
      ElseBB = m_context.builder().GetInsertBlock();

      // Emit merge block.
      TheFunction->getBasicBlockList().push_back( MergeBB );

      m_context.builder().SetInsertPoint( MergeBB );

      auto PN = m_context.builder().CreatePHI(
        llvm::Type::getDoubleTy( m_context ), 2, "iftmp" );

      PN->addIncoming( ThenV, ThenBB );
      PN->addIncoming( ElseV, ElseBB );

      return PN;
    }
  }
}
