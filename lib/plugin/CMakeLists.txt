astateful_add_library(plugin)
add_dependencies(plugin algorithm async BSON mongo token crypto cache script)
include_directories(${SSL_INCLUDE_DIR})