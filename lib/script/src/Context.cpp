#include "astateful/script/Context.hpp"

#include "Lexer.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

namespace astateful {
namespace script {
  struct Context::pimpl {
    llvm::LLVMContext llvm_context;
    builder_t builder;

    pimpl() : builder( llvm_context ) {}
  };

  Context::Context() :
    m_pimpl( std::make_unique<pimpl>() ),
    nextToken( -1 ) {
    m_precedence['='] = 2;
    m_precedence['<'] = 10;
    m_precedence['+'] = 20;
    m_precedence['-'] = 20;
    m_precedence['*'] = 40;
  }

  Context::operator llvm::LLVMContext& () {
    return m_pimpl->llvm_context;
  }

  builder_t& Context::builder() { return m_pimpl->builder; }

  int Context::setNextToken( int next_token ) {

    return ( nextToken = next_token );
  }

  int Context::tokenPrecedence( int token ) {
    if ( !isascii( token ) ) return -1;

    auto token_precendence = m_precedence[token];
    if ( token_precendence <= 0 ) return -1;
    return token_precendence;
  }

  Context::~Context() {}
}
}
