#include "astateful/script/Init.hpp"

#include <llvm/Support/TargetSelect.h>

namespace astateful {
namespace script {
  Init::Init() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
  };
}
}
