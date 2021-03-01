#include "AST/Function.hpp"
#include "AST/Prototype.hpp"
#include "AST/Expression.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>

namespace astateful {
namespace script {
  ASTFunction::ASTFunction( std::unique_ptr<ASTPrototype>& prototype,
                            std::unique_ptr<ASTExpression>& body,
                            Context& context ) :
                            AST( context ),
    m_prototype( std::move( prototype ) ),
    m_body( std::move( body ) ) {}

  llvm::Function * ASTFunction::Codegen() {
    m_context.symbol_table.clear();

    auto TheFunction = m_prototype->Codegen();
    if ( TheFunction == nullptr ) return nullptr;

    // If this is an operator, install it.
    if ( m_prototype->isBinaryOp() )
      m_context.m_precedence[m_prototype->getOperatorName()] = m_prototype->getBinaryPrecedence();

    // Create a new basic block to start insertion into.
    auto BB = llvm::BasicBlock::Create( m_context, "entry", TheFunction );
    m_context.builder().SetInsertPoint( BB );

    // Add all arguments to the symbol table and create their allocas.
    m_prototype->CreateArgumentAllocas( TheFunction );

    if ( auto RetVal = m_body->Codegen() ) {
      // Finish off the function.
      m_context.builder().CreateRet( RetVal );

      // Validate the generated code, checking for consistency.
      verifyFunction( *TheFunction );

      return TheFunction;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();

    if ( m_prototype->isBinaryOp() )
      m_context.m_precedence.erase( m_prototype->getOperatorName() );

    return nullptr;
  }

  ASTFunction * ErrorF( const char *Str ) {
    Error( Str );

    return 0;
  }
}
}
