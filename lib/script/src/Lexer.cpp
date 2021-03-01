#include "Lexer.hpp"

#include <llvm/Support/MemoryBuffer.h>

namespace astateful {
namespace script {
  Lexer::Lexer( const llvm::MemoryBuffer& memory_buffer ) :
    m_position( 0 ),
    m_memory_buffer( memory_buffer ),
    ident( "" ),
    doubleValue( 0.0 ),
    stringValue( "" ),
    m_last_char( ' ' ) {}

  int Lexer::currentChar() {
    auto c = *( m_memory_buffer.getBufferStart() + m_position );
    ++m_position;

    return ( c == '\0' ) ? EOF : c;
  }

  int Lexer::nextToken() {
    // Skip whitespace.
    while ( isspace( m_last_char ) ) m_last_char = currentChar();

    // identifier: [a-zA-Z][a-zA-Z0-9]*
    if ( isalpha( m_last_char ) ) {
      ident = m_last_char;

      while ( isalnum( ( m_last_char = currentChar() ) ) )
        ident += m_last_char;

      if ( ident == "def" ) return tok_def;
      if ( ident == "extern" ) return tok_extern;
      if ( ident == "if" ) return tok_if;
      if ( ident == "then" ) return tok_then;
      if ( ident == "else" ) return tok_else;
      if ( ident == "for" ) return tok_for;
      if ( ident == "in" ) return tok_in;
      if ( ident == "binary" ) return tok_binary;
      if ( ident == "unary" ) return tok_unary;
      if ( ident == "var" ) return tok_var;

      return tok_identifier;
    }

    // Number: [0-9.]+
    if ( isdigit( m_last_char ) || m_last_char == '.' ) {
      std::string NumStr;

      do {
        NumStr += m_last_char;
        m_last_char = currentChar();
      } while ( isdigit( m_last_char ) || m_last_char == '.' );

      doubleValue = std::stod( NumStr );
      return tok_number;
    }

    // String: " [^"]* "
    if ( m_last_char == '"' ) {
      std::string tmp_string = "";
      m_last_char = currentChar();

      do {
        tmp_string += m_last_char;
        m_last_char = currentChar();
      } while ( m_last_char != '"' );

      stringValue = tmp_string;
      return tok_string;
    }

    if ( m_last_char == '#' ) {
      // Comment until end of line.
      do {
        m_last_char = currentChar();
      } while (
        m_last_char != EOF && m_last_char != '\n' && m_last_char != '\r'
      );

      if ( m_last_char != EOF ) return nextToken();
    }

    // Check for end of file.  Don't eat the EOF.
    if ( m_last_char == EOF ) return tok_eof;

    // Return the character as its plain ASCII value. Note that we skip ahead
    // here, storing the next character to be processed.
    auto tc = m_last_char;

    m_last_char = currentChar();

    return tc;
  }
}
}
