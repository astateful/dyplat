#include "AST/Expression/For.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/IRBuilder.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionFor::ASTExpressionFor (
      const std::string& varname,
      std::unique_ptr<ASTExpression>& start,
      std::unique_ptr<ASTExpression>& end,
      std::unique_ptr<ASTExpression>& step,
      std::unique_ptr<ASTExpression>& body,
      Context& context ) :
      ASTExpression( context ),
      VarName( varname ),
      Start( std::move( start ) ),
      End( std::move( end ) ),
      Step( std::move( step ) ),
      Body( std::move( body ) ) {}

    llvm::Value * ASTExpressionFor::Codegen() {
      // Output this as:
      //   var = alloca double
      //   ...
      //   start = startexpr
      //   store start -> var
      //   goto loop
      // loop:
      //   ...
      //   bodyexpr
      //   ...
      // loopend:
      //   step = stepexpr
      //   endcond = endexpr
      //
      //   curvar = load var
      //   nextvar = curvar + step
      //   store nextvar -> var
      //   br endcond, loop, endloop
      // outloop:

      auto TheFunction = m_context.builder().GetInsertBlock ()->getParent ();

      // Create an alloca for the variable in the entry block.
      auto Alloca = CreateEntryBlockAlloca( TheFunction, VarName );

      // Emit the start code first, without 'variable' in scope.
      auto StartVal = Start->Codegen();
      if ( StartVal == 0 ) { return 0; }

      // Store the value into the alloca.
      m_context.builder().CreateStore( StartVal, Alloca );

      // Make the new basic block for the loop header, inserting after current
      // block.
      auto LoopBB = llvm::BasicBlock::Create( m_context, "loop", TheFunction );

      // Insert an explicit fall through from the current block to the LoopBB.
      m_context.builder().CreateBr( LoopBB );

      // Start insertion in LoopBB.
      m_context.builder().SetInsertPoint( LoopBB );

      // Within the loop, the variable is defined equal to the PHI node.  If it
      // shadows an existing variable, we have to restore it, so save it now.
      auto OldVal = m_context.symbol_table[VarName];
      m_context.symbol_table[VarName] = Alloca;

      // Emit the body of the loop.  This, like any other expr, can change the
      // current BB.  Note that we ignore the value computed by the body, but don't
      // allow an error.
      if ( Body->Codegen() == 0 ) return 0;

      // Emit the step value.
      llvm::Value *StepVal;
      if ( Step )
      {
        StepVal = Step->Codegen();
        if ( StepVal == 0 ) { return 0; }
      }
      else
      {
        // If not specified, use 1.0.
        StepVal = llvm::ConstantFP::get( m_context, llvm::APFloat( 1.0 ) );
      }

      // Compute the end condition.
      llvm::Value *EndCond = End->Codegen();
      if ( EndCond == 0 ) { return EndCond; }

      // Reload, increment, and restore the alloca.  This handles the case where
      // the body of the loop mutates the variable.
      llvm::Value *CurVar = m_context.builder().CreateLoad( Alloca, VarName.c_str() );
      llvm::Value *NextVar = m_context.builder().CreateFAdd( CurVar, StepVal, "nextvar" );
      m_context.builder().CreateStore( NextVar, Alloca );

      // Convert condition to a bool by comparing equal to 0.0.
      EndCond = m_context.builder().CreateFCmpONE( EndCond,
        llvm::ConstantFP::get( m_context, llvm::APFloat( 0.0 ) ),
                                         "loopcond" );

      // Create the "after loop" block and insert it.
      llvm::BasicBlock *AfterBB = llvm::BasicBlock::Create(
        m_context, "afterloop", TheFunction );

      // Insert the conditional branch into the end of LoopEndBB.
      m_context.builder().CreateCondBr( EndCond, LoopBB, AfterBB );

      // Any new code will be inserted in AfterBB.
      m_context.builder().SetInsertPoint( AfterBB );

      // Restore the unshadowed variable.
      if ( OldVal )
      {
        m_context.symbol_table[VarName] = OldVal;
      }
      else
      {
        m_context.symbol_table.erase( VarName );
      }

      // for expr always returns 0.0.
      return llvm::Constant::getNullValue( llvm::Type::getDoubleTy( m_context ) );
    }
  }
}
