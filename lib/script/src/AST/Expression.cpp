#include "AST/Expression.hpp"

namespace astateful {
namespace script {
  ASTExpression::ASTExpression( Context& context ) : AST( context ) {}

  std::unique_ptr<ASTExpression> Error( const char *Str )
  {
    fprintf( stderr, "Error: %s\n", Str );

    return nullptr;
  }

  llvm::Value * ErrorV ( const char *Str )
  {
    Error( Str );

    return 0;
  }
}
}