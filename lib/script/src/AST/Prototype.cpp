#include "AST/Prototype.hpp"
#include "AST/Expression.hpp"

#include "astateful/script/Context.hpp"
#include "astateful/script/JIT.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

#include <cassert>

namespace astateful
{
  namespace script
  {
    ASTPrototype::ASTPrototype (
      const std::string& name,
      const std::vector<std::string>& args,
      JIT& cJIT,
      Context& context,
      bool isoperator,
      unsigned prec ) :
      AST( context ),
      Name( name ),
      Args( args ),
      mcJIT( cJIT ),
      isOperator( isoperator ),
      Precedence( prec ) {}

    char ASTPrototype::getOperatorName() const
    {
      assert( isUnaryOp() || isBinaryOp() );
      return Name[Name.size() - 1];
    }

    llvm::Function * ASTPrototype::Codegen ()
    {
      // Make the function type:  double(double,double) etc.
      std::vector<llvm::Type*> Doubles ( Args.size (), llvm::Type::getDoubleTy ( m_context ) );
      auto FT = llvm::FunctionType::get( llvm::Type::getDoubleTy( m_context ), Doubles, false );

      auto FnName = MakeLegalFunctionName( Name );

      auto M = mcJIT.getModuleForNewFunction ();

      auto F = llvm::Function::Create( FT, llvm::Function::ExternalLinkage, FnName, M );

      // If F conflicted, there was already something named 'FnName'.  If it has a
      // body, don't allow redefinition or reextern.
      if ( F->getName () != FnName )
      {
        // Delete the one we just made and get the existing one.
        F->eraseFromParent ();
        F = M->getFunction ( Name );

        // If F already has a body, reject this.
        if ( !F->empty () )
        {
          ErrorF ( "redefinition of function" );
          return 0;
        }

        // If F took a different number of args, reject.
        if ( F->arg_size () != Args.size () )
        {
          ErrorF ( "redefinition of function with different # args" );
          return 0;
        }
      }

      // Set names for all arguments.
      unsigned Idx = 0;
      for ( auto AI = F->arg_begin(); Idx != Args.size(); ++AI, ++Idx ) {
        AI->setName ( Args[Idx] );
      }

      return F;
    }

    /// CreateArgumentAllocas - Create an alloca for each argument and register the
    /// argument in the symbol table so that references to it will succeed.
    void ASTPrototype::CreateArgumentAllocas ( llvm::Function * F )
    {
      auto AI = F->arg_begin();
      for ( unsigned Idx = 0, e = Args.size(); Idx != e; ++Idx, ++AI )
      {
        // Create an alloca for this variable.
        auto Alloca = CreateEntryBlockAlloca( F, Args [Idx] );

        // Store the initial value into the alloca.
        m_context.builder().CreateStore ( AI, Alloca );

        // Add arguments to variable symbol table.
        m_context.symbol_table[Args[Idx]] = Alloca;
      }
    }

    std::unique_ptr<ASTPrototype> ErrorP( const char *Str )
    {
      Error( Str );

      return nullptr;
    }
  }
}
