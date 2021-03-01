#include "AST/Expression/Var.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/IRBuilder.h>

namespace astateful
{
  namespace script
  {
    ASTExpressionVar::ASTExpressionVar (
      std::vector <std::pair<std::string, std::unique_ptr<ASTExpression>> >& varnames,
      std::unique_ptr<ASTExpression>& body,
      Context& context ) :
      ASTExpression( context ),
      VarNames( std::move( varnames ) ),
      Body( std::move( body ) ) {}

    llvm::Value * ASTExpressionVar::Codegen ()
    {
      std::vector<llvm::AllocaInst *> OldBindings;

      auto TheFunction = m_context.builder().GetInsertBlock ()->getParent ();

      // Register all variables and emit their initializer.
      for ( unsigned i = 0, e = VarNames.size(); i != e; ++i )
      {
        const auto& VarName = VarNames[i].first;
        ASTExpression *Init = VarNames[i].second.get();

        // Emit the initializer before adding the variable to scope, this prevents
        // the initializer from referencing the variable itself, and permits stuff
        // like this:
        //  var a = 1 in
        //    var a = a in ...   # refers to outer 'a'.
        llvm::Value *InitVal;
        if ( Init )
        {
          InitVal = Init->Codegen();
          if ( InitVal == 0 ) { return 0; }
        }
        else     // If not specified, use 0.0.
        {
          InitVal = llvm::ConstantFP::get (
            m_context, llvm::APFloat( 0.0 ) );
        }

        auto Alloca = CreateEntryBlockAlloca( TheFunction, VarName );

        m_context.builder().CreateStore( InitVal, Alloca );

        // Remember the old variable binding so that we can restore the binding when
        // we unrecurse.
        OldBindings.push_back( m_context.symbol_table[VarName] );

        // Remember this binding.
        m_context.symbol_table[VarName] = Alloca;
      }

      // Codegen the body, now that all vars are in scope.
      auto BodyVal = Body->Codegen();
      if ( BodyVal == 0 ) { return 0; }

      // Pop all our variables from scope.
      for ( unsigned i = 0, e = VarNames.size(); i != e; ++i )
      {
        m_context.symbol_table[VarNames[i].first] = OldBindings[i];
      }

      // Return the body computation.
      return BodyVal;
    }
  }
}
