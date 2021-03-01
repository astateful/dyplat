find_package(LLVM REQUIRED CONFIG)
astateful_add_library(script)
include_directories(${LLVM_INCLUDE_DIRS})