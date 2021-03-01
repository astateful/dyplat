#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/script/JIT.hpp"
#include "astateful/script/Init.hpp"
#include "astateful/script/Context.hpp"

#include "Lexer.hpp"
#include "Parse.hpp"

#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>

using namespace astateful;
using namespace astateful::script;

typedef double ( *f_ptr_t )( double );

void test_jit( JIT& jit ) {
  SECTION( "recursive" ) {
    auto fn = jit.getFunction( "fib" );
    auto void_f_ptr = jit.getPointerToFunction( fn );
    f_ptr_t f_ptr = static_cast<f_ptr_t>( void_f_ptr );

    REQUIRE( f_ptr( 10 ) == 55.000000 );
  }

  SECTION( "iterative" ) {
    auto fn = jit.getFunction( "iter" );
    auto void_f_ptr = jit.getPointerToFunction( fn );
    f_ptr_t f_ptr = static_cast<f_ptr_t>( void_f_ptr );

    REQUIRE( f_ptr( 10 ) == 55.000000 );
  }
}

TEST_CASE( "jit" ) {
  std::string code( "def binary : 1 (x y) y;" );
  code += "def fib(x) if (x < 3) then 1 else fib(x-1) + fib(x-2);";
  code += "def iter(x) var a = 1, b = 1, c in (for i = 3, i < x in c = a + b : a = b : b = c) : b;";
  //code += "var x = \"abc\";";

  auto src_buffer = llvm::MemoryBuffer::getMemBuffer( code );

  script::Init init;
  script::Context src_context;
  script::JIT jit_empty( src_context );
  script::Lexer lexer( *src_buffer.get() );
  script::Parse parse;
  parse( jit_empty, src_context, lexer );

  test_jit( jit_empty );

  SECTION( "ir parsing" ) {
    std::string ir_output;
    llvm::raw_string_ostream stream( ir_output );

    REQUIRE( jit_empty.output( stream ) );

    auto ir_buffer = llvm::MemoryBuffer::getMemBuffer( ir_output );

    script::Context ir_context;
    script::JIT jit_ir( ir_context, *ir_buffer.get() );

    test_jit( jit_ir );
  }
}
