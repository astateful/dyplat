#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace llvm {
  class MemoryBuffer;
}

namespace astateful {
namespace script {
  //! The lexer returns tokens [0-255] if it is an unknown character, otherwise
  //! one of these for known things.
  enum Token {
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,
    tok_if = -6,
    tok_then = -7,
    tok_else = -8,
    tok_for = -9,
    tok_in = -10,
    tok_binary = -11,
    tok_unary = -12,
    tok_var = -13,
    tok_string = -14
  };

  struct Lexer final {
    //! Populated if we have a token of type tok_identifier;
    std::string ident;

    //! Populated if we hve a token of type tok_number.
    double doubleValue;

    //! Populated if we have a token of type tok_string.
    std::string stringValue;

    //! Initialize the lexer with a memory buffer. The buffer is general enough
    //! that it can point to a file source, raw data source, string, etc.
    //!
    //! @param memory_buffer A generalized memory block that can be tokenized.
    //!
    Lexer( const llvm::MemoryBuffer& memory_buffer );

    //! Return the next token from the input memory buffer. This works by first
    //! starting with the character at the current position and reading into an
    //! identifier until the next valid token is identified. If there is no
    //! next token, the last char is nonetheless incremented and current char is
    //! returned.
    //!
    //! @return The last token which a valid token or the last character.
    //!
    int nextToken();
  private:
    //! Stores the position of the current character to process.
    int m_position;

    //! A generalized memory block that can be tokenized.
    const llvm::MemoryBuffer& m_memory_buffer;

    //! This is the last character that was read, not processed.
    int m_last_char;

    //! Retrieve the character referenced by the current position and then
    //! increment the current position. Returns EOF if the last char is \0;
    //! this is possible due to the implementation of MemoryBuffer.
    //!
    //! @return The char at the current position, EOF if char is \0.
    //!
    int currentChar();
  };
}
}
