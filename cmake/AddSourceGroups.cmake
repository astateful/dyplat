macro(astateful_add_library lib)
  as_group_public_headers(${PROJECT_SOURCE_DIR}/include/astateful/${lib} ${lib})
  as_group_private_headers(${PROJECT_SOURCE_DIR}/lib/${lib}/include ${lib} lib)
  as_group_sources(${PROJECT_SOURCE_DIR}/lib/${lib}/src ${lib} lib)
  as_group_autogen_sources(${CMAKE_BINARY_DIR}/autogen/src/${lib} ${lib})
  as_group_autogen_headers(${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib} ${lib})
	
	file( GLOB_RECURSE sources
  ${PROJECT_SOURCE_DIR}/include/astateful/${lib}/*.hpp
  ${PROJECT_SOURCE_DIR}/lib/${lib}/include/*.hpp
	${PROJECT_SOURCE_DIR}/lib/${lib}/src/*.cpp
  ${CMAKE_BINARY_DIR}/autogen/src/${lib}/*.cpp
  ${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib}/*.hpp )
	
	add_library(${lib} ${sources})
	SET_PROPERTY(TARGET ${lib} PROPERTY FOLDER "Libraries")
	include_directories("include")
  include_directories(${PROJECT_SOURCE_DIR}/include)
  include_directories(${CMAKE_BINARY_DIR}/autogen/include)
endmacro()

macro(astateful_add_ho_library lib)
  as_group_public_headers(${PROJECT_SOURCE_DIR}/include/astateful/${lib} ${lib})
	file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/include/astateful/${lib}/*.hpp)
	add_custom_target(${lib} SOURCES ${sources})
	SET_PROPERTY(TARGET ${lib} PROPERTY FOLDER "Libraries")
endmacro()

macro(astateful_add_test)
	set(lib ${ARGV0})
	
	foreach(f ${ARGN})
		if (${f} STREQUAL "ssl")
			set(enable_ssl 1)
		endif()
		if (${f} STREQUAL "net")
			set(enable_net 1)
		endif()
	endforeach()

	as_group_private_headers(${PROJECT_SOURCE_DIR}/unittests/${lib}/include ${lib} unittests)
	as_group_sources(${PROJECT_SOURCE_DIR}/unittests/${lib}/src ${lib} unittests)
	
	file( GLOB_RECURSE sources 
  ${PROJECT_SOURCE_DIR}/unittests/${lib}/include/*.hpp
  ${PROJECT_SOURCE_DIR}/unittests/${lib}/src/*.cpp )
	
	if (enable_ssl)
		link_directories(${SSL_LIB_DIR})
	endif()
	
	add_executable(${lib}Tests ${sources})
	
	SET_PROPERTY(TARGET ${lib}Tests PROPERTY FOLDER "Tests")
	
	add_test(NAME ${lib}Tests COMMAND ${lib}Tests)
	
	include_directories(${CATCH_INCLUDE_DIR})
  include_directories(${PROJECT_SOURCE_DIR}/include)
	
	if (enable_ssl)
		target_link_libraries(${lib}Tests libsslMDd.lib libcryptoMDd.lib crypt32.lib)
    target_link_libraries(${lib}Tests legacy_stdio_definitions.lib)
    #include_directories(${SSL_INCLUDE_DIR})
	endif()
  
	if (enable_net)
		target_link_libraries(${lib}Tests ws2_32.lib Shlwapi.lib)
	endif()
	
	include_directories(${CMAKE_BINARY_DIR}/autogen/include)
  include_directories(${PROJECT_SOURCE_DIR}/lib/${lib}/include)
endmacro()

macro(astateful_target_test)
	set(lib ${ARGV0})
	foreach(f ${ARGN})
		target_link_libraries(${lib}Tests ${f})
	endforeach()
endmacro()

macro(astateful_add_example)
	set(name ${ARGV0})
	
	foreach(f ${ARGN})
		if (${f} STREQUAL "ssl")
			set(enable_ssl 1)
		endif()
	endforeach()

	as_group_private_headers(${PROJECT_SOURCE_DIR}/examples/${name}/include ${name} examples)
	as_group_sources(${PROJECT_SOURCE_DIR}/examples/${name}/src ${name} examples)
  
	file( GLOB_RECURSE sources
  ${PROJECT_SOURCE_DIR}/examples/${name}/include/*.hpp
  ${PROJECT_SOURCE_DIR}/examples/${name}/src/*.cpp )
  
	if (enable_ssl)
		link_directories(${SSL_LIB_DIR})
	endif()
  
	add_executable(${name} ${sources})
  
	SET_PROPERTY(TARGET ${name} PROPERTY FOLDER "Examples")
  
	if (enable_ssl)
		include_directories(${PROJECT_SOURCE_DIR}/lib/ssl/include)
		target_link_libraries(${name} libssl64MDd.lib libcrypto64MD.lib)
	endif()
  
	include_directories(${CMAKE_BINARY_DIR}/autogen/include)
  include_directories(${PROJECT_SOURCE_DIR}/include)
endmacro()