#pragma once

#include "../AST.hpp"

#include <vector>
#include <memory>

namespace astateful
{
  namespace script
  {
    struct JIT;

    struct ASTFunction;

    /// PrototypeAST - This class represents the "prototype" for a function,
    /// which captures its argument names as well as if it is an operator.
    class ASTPrototype final : public AST
    {
      std::string Name;

      std::vector<std::string> Args;

      bool isOperator;

      // Precedence if a binary op.
      unsigned Precedence;

      //!
      JIT& mcJIT;
    public:
      ASTPrototype(
        const std::string& name,
        const std::vector<std::string>& args,
        JIT&,
        Context& context,
        bool isoperator = false,
        unsigned prec = 0
      );

      bool isUnaryOp() const { return isOperator && Args.size() == 1; }

      bool isBinaryOp() const { return isOperator && Args.size() == 2; }

      char getOperatorName() const;

      unsigned getBinaryPrecedence() const { return Precedence; }

      llvm::Function *Codegen();

      void CreateArgumentAllocas( llvm::Function *F );
    };

    std::unique_ptr<ASTPrototype> ErrorP( const char * );

    ASTFunction * ErrorF( const char * );
  };
};
