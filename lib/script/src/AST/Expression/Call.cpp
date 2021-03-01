#include "AST/Expression/Call.hpp"

#include "astateful/script/JIT.hpp"
#include "astateful/script/Context.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionCall::ASTExpressionCall (
      const std::string& callee,
      std::vector<std::unique_ptr<ASTExpression>>& args,
      JIT& cJIT,
      Context& context ) :
      ASTExpression( context ),
      Callee( callee ),
      Args( std::move( args ) ),
      mcJIT( cJIT ) {}

    llvm::Value * ASTExpressionCall::Codegen ()
    {
      // Look up the name in the global module table.
      llvm::Function * CalleeF = mcJIT.getFunction ( Callee );
      if ( CalleeF == 0 )
      {
        return ErrorV( "Unknown function referenced" );
      }

      // If argument mismatch error.
      if ( CalleeF->arg_size() != Args.size() )
      {
        return ErrorV( "Incorrect # arguments passed" );
      }

      std::vector<llvm::Value*> ArgsV;
      for ( unsigned i = 0, e = Args.size(); i != e; ++i )
      {
        ArgsV.push_back( Args[i]->Codegen() );
        if ( ArgsV.back() == 0 ) { return 0; }
      }

      return m_context.builder().CreateCall( CalleeF, ArgsV, "calltmp" );
    }
  }
}
