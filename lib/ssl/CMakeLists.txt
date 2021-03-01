astateful_add_library(SSL)
add_dependencies(plugin algorithm async BSON mongo token crypto cache script plugin protocol)
include_directories(${SSL_INCLUDE_DIR})