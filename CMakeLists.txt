cmake_minimum_required( VERSION 2.6 )

SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

set( SSL_INCLUDE_DIR "" CACHE PATH "" )
set( SSL_LIB_DIR "" CACHE PATH "" )
set( CATCH_INCLUDE_DIR "" CACHE PATH "" )
set( CMAKE_PREFIX_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )


add_definitions( -DNOMINMAX )
add_definitions( -D_SCL_SECURE_NO_WARNINGS )

STRING( REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ) # Disable RTTI

project( astateful C CXX )

include( CTest )

include( ${CMAKE_SOURCE_DIR}/cmake/ConfigureSourceGroups.cmake )
include( ${CMAKE_SOURCE_DIR}/cmake/AddSourceGroups.cmake )
include( ${CMAKE_SOURCE_DIR}/cmake/ConfigureFSM.cmake )

add_subdirectory( lib )
add_subdirectory( unittests )
add_subdirectory( examples )

configure_file( config.h.in ${CMAKE_BINARY_DIR}/autogen/include/autogen/config.h )