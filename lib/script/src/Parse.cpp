#include "astateful/script/JIT.hpp"
#include "astateful/script/Context.hpp"

#include "Parse.hpp"
#include "Lexer.hpp"
#include "AST/Function.hpp"
#include "AST/Prototype.hpp"
#include "AST/Expression/Variable.hpp"
#include "AST/Expression/Call.hpp"
#include "AST/Expression/Number.hpp"
#include "AST/Expression/If.hpp"
#include "AST/Expression/For.hpp"
#include "AST/Expression/Var.hpp"
#include "AST/Expression/Unary.hpp"
#include "AST/Expression/Binary.hpp"

namespace astateful {
namespace script {
namespace {
  std::unique_ptr<ASTExpression> parse_exp( Lexer&, JIT&, Context& );

  std::unique_ptr<ASTExpression> parse_number_expr( Lexer& lexer,
    Context& context ) {
    auto Result = std::make_unique<ASTExpressionNumber>(
      lexer.doubleValue, context );

    context.nextToken = lexer.nextToken(); // Consume the number.

    return std::move( Result );
  }

  std::unique_ptr<ASTPrototype> parse_prototype( Lexer& lexer,
    JIT& jit, Context& context ) {
    std::string FnName;
    unsigned Kind = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned BinaryPrecedence = 30;

    switch ( context.nextToken )
    {
    default:
      return ErrorP( "Expected function name in prototype" );
      break;
    case tok_identifier:
      FnName = lexer.ident;
      Kind = 0;
      context.nextToken = lexer.nextToken();
      break;
    case tok_unary:
      context.nextToken = lexer.nextToken();
      if ( !isascii( context.nextToken ) ) return ErrorP( "Expected unary operator" );
      FnName = "unary";
      FnName += ( char ) context.nextToken;
      Kind = 1;
      context.nextToken = lexer.nextToken();
      break;
    case tok_binary:
      context.nextToken = lexer.nextToken();
      if ( !isascii( context.nextToken ) ) return ErrorP( "Expected binary operator" );
      FnName = "binary";
      FnName += ( char ) context.nextToken;
      Kind = 2;

      context.nextToken = lexer.nextToken();

      // Read the precedence if present.
      if ( context.nextToken == tok_number ) {
        if ( lexer.doubleValue < 1 || lexer.doubleValue > 100 )
          return ErrorP( "Invalid precedecnce: must be 1..100" );

        BinaryPrecedence = static_cast<unsigned>( lexer.doubleValue );

        context.nextToken = lexer.nextToken();
      }
      break;
    }

    if ( context.nextToken != '(' ) return ErrorP( "Expected '(' in prototype" );

    std::vector<std::string> ArgNames;

    while ( ( context.nextToken = lexer.nextToken() ) == tok_identifier )
      ArgNames.push_back( lexer.ident );

    if ( context.nextToken != ')' ) return ErrorP( "Expected ')' in prototype" );

    // success.
    context.nextToken = lexer.nextToken(); // eat ')'.

    // Verify right number of names for operator.
    if ( Kind && ArgNames.size() != Kind )
      return ErrorP( "Invalid number of operands for operator" );

    return std::make_unique<ASTPrototype>(
      FnName,
      ArgNames,
      jit,
      context,
      Kind != 0,
      BinaryPrecedence );
  }

  std::unique_ptr<ASTFunction> parse_definition( Lexer& lexer,
    JIT& jit, Context& context ) {
    context.nextToken = lexer.nextToken(); // eat def.

    auto prototype = parse_prototype( lexer, jit, context );
    if ( !prototype ) return nullptr;

    auto expression = parse_exp( lexer, jit, context );
    if ( !expression ) return nullptr;

    return std::make_unique<ASTFunction>( prototype,
      expression,
      context );
  }

  std::unique_ptr<ASTFunction> parse_toplevel_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    auto E = parse_exp( lexer, jit, context );
    if ( !E ) return nullptr;

    auto Proto = std::make_unique<ASTPrototype>(
      "",
      std::vector<std::string>(),
      jit,
      context );

    return std::make_unique<ASTFunction>(
      Proto,
      E,
      context );
  }

  std::unique_ptr<ASTPrototype> parse_extern( Lexer& lexer, JIT& jit,
    Context& context ) {
    context.nextToken = lexer.nextToken(); // Eat extern.
    return std::move( parse_prototype( lexer, jit, context ) );
  }

  std::unique_ptr<ASTExpression> parse_ident_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    auto IdName = lexer.ident;

    context.nextToken = lexer.nextToken(); // eat identifier.

    if ( context.nextToken != '(' ) // Simple variable ref.
      return std::make_unique<ASTExpressionVariable>(
      context,
      IdName );

    // Call.
    context.nextToken = lexer.nextToken(); // eat (

    std::vector<std::unique_ptr<ASTExpression>> Args;

    if ( context.nextToken != ')' ) {
      while ( true ) {
        auto Arg = parse_exp( lexer, jit, context );
        if ( !Arg ) return nullptr;

        Args.push_back( std::move( Arg ) );

        if ( context.nextToken == ')' ) break;

        if ( context.nextToken != ',' )
          return std::move( Error( "Expected ')' or ',' in argument list" ) );

        context.nextToken = lexer.nextToken();
      }
    }

    // Eat the ')'.
    context.nextToken = lexer.nextToken();

    return std::make_unique<ASTExpressionCall>(
      IdName,
      Args,
      jit,
      context );
  }

  std::unique_ptr<ASTExpression> parse_paren_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    context.nextToken = lexer.nextToken(); // eat (

    std::unique_ptr<ASTExpression> V = parse_exp( lexer, jit, context );
    if ( !V ) return nullptr;

    if ( context.nextToken != ')' ) return std::move( Error( "expected ')'" ) );

    context.nextToken = lexer.nextToken(); // eat )

    return std::move( V );
  }

  std::unique_ptr<ASTExpression> parse_if_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    // eat the if.
    context.nextToken = lexer.nextToken();

    // condition.
    auto Cond = parse_exp( lexer, jit, context );
    if ( !Cond ) { return nullptr; }

    if ( context.nextToken != tok_then ) return std::move( Error( "expected then" ) );

    context.nextToken = lexer.nextToken(); // eat the then

    auto Then = parse_exp( lexer, jit, context );
    if ( Then == 0 ) { return nullptr; }

    if ( context.nextToken != tok_else ) return std::move( Error( "expected else" ) );

    context.nextToken = lexer.nextToken();

    auto Else = parse_exp( lexer, jit, context );
    if ( !Else ) { return nullptr; }

    return std::make_unique<ASTExpressionIf>( Cond, Then, Else, context );
  }

  std::unique_ptr<ASTExpression> parse_for_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    context.nextToken = lexer.nextToken(); // eat the for.

    if ( context.nextToken != tok_identifier )
      return std::move( Error( "expected identifier after for" ) );

    auto IdName = lexer.ident;
    context.nextToken = lexer.nextToken(); // eat identifier.

    if ( context.nextToken != '=' )
      return std::move( Error( "expected '=' after for" ) );

    context.nextToken = lexer.nextToken(); // eat '='.

    auto Start = parse_exp( lexer, jit, context );

    if ( !Start ) { return nullptr; }
    if ( context.nextToken != ',' )
      return std::move( Error( "expected ',' after for start value" ) );

    context.nextToken = lexer.nextToken();

    auto End = parse_exp( lexer, jit, context );
    if ( !End ) { return nullptr; }

    // The step value is optional.
    std::unique_ptr<ASTExpression> Step;
    if ( context.nextToken == ',' )
    {
      context.nextToken = lexer.nextToken();
      Step = parse_exp( lexer, jit, context );
      if ( Step == 0 ) { return 0; }
    }

    if ( context.nextToken != tok_in )
      return std::move( Error( "expected 'in' after for" ) );

    context.nextToken = lexer.nextToken(); // eat 'in'.

    auto Body = parse_exp( lexer, jit, context );
    if ( !Body ) { return nullptr; }

    return std::make_unique<ASTExpressionFor>(
      IdName,
      Start,
      End,
      Step,
      Body,
      context );
  }

  std::unique_ptr<ASTExpression> parse_var_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    context.nextToken = lexer.nextToken(); // eat the var.

    std::vector<std::pair<std::string, std::unique_ptr<ASTExpression>> > VarNames;

    // At least one variable name is required.
    if ( context.nextToken != tok_identifier )
      return std::move( Error( "expected identifier after var" ) );

    while ( true ) {
      auto Name = lexer.ident;

      // eat identifier.
      context.nextToken = lexer.nextToken();

      // Read the optional initializer.
      std::unique_ptr<ASTExpression> assign;
      if ( context.nextToken == '=' )
      {
        // eat the '='.
        context.nextToken = lexer.nextToken();

        assign = parse_exp( lexer, jit, context );
        if ( !assign ) { return nullptr; }
      }

      VarNames.push_back( std::make_pair( Name, std::move( assign ) ) );

      // End of var list, exit loop.
      if ( context.nextToken != ',' ) { break; }
      context.nextToken = lexer.nextToken(); // eat the ','.

      if ( context.nextToken != tok_identifier )
        return std::move( Error( "expected identifier list after var" ) );
    }

    // At this point, we have to have 'in'.
    if ( context.nextToken != tok_in )
      return std::move( Error( "expected 'in' keyword after 'var'" ) );

    // eat 'in'.
    context.nextToken = lexer.nextToken();

    std::unique_ptr<ASTExpression> Body( parse_exp( lexer, jit, context ) );
    if ( !Body ) return nullptr;

    return std::make_unique<ASTExpressionVar>(
      VarNames,
      Body,
      context );
  }

  std::unique_ptr<ASTExpression> parse_primary( Lexer& lexer, JIT& jit,
    Context& context ) {
    switch ( context.nextToken ) {
    case tok_identifier:
      return std::move( parse_ident_exp( lexer, jit, context ) );
      break;
    case tok_number:
      return std::move( parse_number_expr( lexer, context ) );
      break;
    case '(':
      return std::move( parse_paren_exp( lexer, jit, context ) );
      break;
    case tok_if:
      return std::move( parse_if_exp( lexer, jit, context ) );
      break;
    case tok_for:
      return std::move( parse_for_exp( lexer, jit, context ) );
      break;
    case tok_var:
      return std::move( parse_var_exp( lexer, jit, context ) );
      break;
    default:
      return std::move( Error( "unknown token when expecting an expression" ) );
      break;
    }
  }

  std::unique_ptr<ASTExpression> parse_unary( Lexer& lexer, JIT& jit,
    Context& context ) {
    // If the current token is not an operator, it must be a primary expr.
    if ( !isascii( context.nextToken ) || context.nextToken == '(' || context.nextToken == ',' )
      return parse_primary( lexer, jit, context );

    // If this is a unary operator, read it.
    int Opc = context.nextToken;

    context.nextToken = lexer.nextToken();

    auto Operand = parse_unary( lexer, jit, context );
    if ( !Operand ) return nullptr;

    return std::make_unique<ASTExpressionUnary>(
      Opc,
      Operand,
      jit,
      context );
  }

  std::unique_ptr<ASTExpression> parse_bin_op_rhs( int ExprPrec,
    std::unique_ptr<ASTExpression>& LHS, Lexer& lexer, JIT& jit,
    Context& context ) {
    // If this is a binop, find its precedence.
    while ( true )
    {
      int TokPrec = context.tokenPrecedence( context.nextToken );

      // If this is a binop that binds at least as tightly as the current binop,
      // consume it, otherwise we are done.
      if ( TokPrec < ExprPrec ) return std::move( LHS );

      // Okay, we know this is a binop.
      int BinOp = context.nextToken;
      context.nextToken = lexer.nextToken(); // eat binop

      // Parse the unary expression after the binary operator.
      auto RHS = parse_unary( lexer, jit, context );
      if ( !RHS ) { return nullptr; }

      // If BinOp binds less tightly with RHS than the operator after RHS, let
      // the pending operator take RHS as its LHS.
      int NextPrec = context.tokenPrecedence( context.nextToken );
      if ( TokPrec < NextPrec )
      {
        RHS = parse_bin_op_rhs( TokPrec + 1, RHS, lexer, jit, context );
        if ( RHS == 0 ) { return 0; }
      }

      // Merge LHS/RHS.
      LHS = std::make_unique<ASTExpressionBinary>(
        BinOp,
        LHS,
        RHS,
        jit,
        context );
    }
  }

  std::unique_ptr<ASTExpression> parse_exp( Lexer& lexer, JIT& jit,
    Context& context ) {
    auto LHS = parse_unary( lexer, jit, context );
    if ( !LHS ) return nullptr;

    return std::move( parse_bin_op_rhs( 0, LHS, lexer, jit, context ) );
  }
}

  void Parse::operator()( JIT& jit, Context& context, Lexer& lexer ) {
    context.nextToken = lexer.nextToken(); // "Eat" the first token.

    while ( true ) {
      switch ( context.nextToken ) {
      case tok_eof:
        return;
        break;
        // Ignore top-level semicolons.
      case ';':
        context.nextToken = lexer.nextToken();
        break;
      case tok_def: {
        auto result = parse_definition( lexer, jit, context );
        if ( result ) {
          auto LF = result->Codegen();
        } else {
          // Skip token for error recovery.
          context.nextToken = lexer.nextToken();
        }
      }
      break;
      case tok_extern: {
        auto result = parse_extern( lexer, jit, context );
        if ( result ) {
          if ( auto F = result->Codegen() ) {
            //fprintf(stderr, "Read extern: ");
            //F->dump();
          }
        } else {
          // Skip token for error recovery.
          context.nextToken = lexer.nextToken();
        }
      }
      break;
      default: {
        // Evaluate a top-level expression into an anonymous function.
        auto result = parse_toplevel_exp( lexer, jit, context );
        if ( result ) {
          if ( auto LF = result->Codegen() ) {
            // JIT the function, returning a function pointer.
            auto FPtr = jit.getPointerToFunction( LF );
            // Cast it to the right type (takes no arguments, returns a double) so we
            // can call it as a native function.
            double( *FP )( ) = ( double( *)( ) )( intptr_t ) FPtr;
            double Result = FP();
            //if (VerboseOutput)
            //fprintf (stderr, "Evaluated to %f\n", Result);

            context.msOutput += std::to_string( Result );
          }
        } else {
          // Skip token for error recovery.
          context.nextToken = lexer.nextToken();
        }
      }
      break;
      }
    }
  }
}
}

/// putchard - putchar that takes a double and returns 0.
extern "C" double putchard ( double X )
{
  putchar( ( char )X );
  return 0;
};

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" double printd ( double X )
{
  printf( "%f\n", X );
  return 0;
}