astateful_add_library(protocol)
add_dependencies(plugin algorithm async BSON mongo token crypto cache script plugin)
include_directories(${SSL_INCLUDE_DIR})