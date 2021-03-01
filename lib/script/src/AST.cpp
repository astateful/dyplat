#include "AST.hpp"

#include "astateful/script/Context.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

namespace astateful {
namespace script {
  AST::AST( Context& context ) : m_context( context ) {}

  llvm::AllocaInst * AST::CreateEntryBlockAlloca( llvm::Function * TheFunction,
    const std::string& VarName ) {
    llvm::IRBuilder<> TmpB( &TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin() );

    return TmpB.CreateAlloca( llvm::Type::getDoubleTy( m_context ),
              0, VarName.c_str() );
  }

  std::string AST::MakeLegalFunctionName( std::string Name ) {
    std::string NewName;
    if ( !Name.length() ) return GenerateUniqueName( "anon_func_" );

    // Start with what we have
    NewName = Name;

    // Look for a numberic first character
    if ( NewName.find_first_of( "0123456789" ) == 0 )
      NewName.insert( 0, 1, 'n' );

    // Replace illegal characters with their ASCII equivalent
    std::string legal_elements = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t pos;
    while ( ( pos = NewName.find_first_not_of( legal_elements ) ) != std::string::npos )
    {
      char old_c = NewName.at( pos );
      char new_str[16];
      sprintf( new_str, "%d", ( int )old_c );
      NewName = NewName.replace( pos, 1, new_str );
    }

    return NewName;
  }

  std::string AST::GenerateUniqueName( const char *root ) {
    static int i = 0;
    char s[16];
    sprintf( s, "%s%d", root, i++ );
    std::string S = s;
    return S;
  }
}
}
