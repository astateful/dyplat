macro( as_group_sources curdir lib folder )
	file( GLOB children ${curdir}/* )
	foreach( child ${children} )
		if( IS_DIRECTORY ${child} )
			as_group_sources( ${child} ${lib} ${folder} )
		else()
			string( REPLACE ${PROJECT_SOURCE_DIR}/${folder}/${lib}/src "/" res ${curdir} )
			string( REPLACE "/" "\\" groupname ${res} )
      source_group( src\\${groupname} FILES ${child} )
    endif()
  endforeach()
endmacro()

macro( as_group_public_headers curdir lib )
	file( GLOB children ${curdir}/* )
	foreach( child ${children} )
		if( IS_DIRECTORY ${child} )
			as_group_public_headers( ${child} ${lib} )
		else()
			string( REPLACE ${PROJECT_SOURCE_DIR}/include/astateful/${lib} "/" res ${curdir} )
			string( REPLACE "/" "\\" groupname ${res} )
      source_group( header\\${groupname} FILES ${child} )
    endif()
  endforeach()
endmacro()

macro( as_group_private_headers curdir lib folder)
	file( GLOB children ${curdir}/* )
	foreach( child ${children} )
		if( IS_DIRECTORY ${child} )
			as_group_private_headers( ${child} ${lib} ${folder} )
		else()
			string( REPLACE ${PROJECT_SOURCE_DIR}/${folder}/${lib}/include "/" res ${curdir} )
			string( REPLACE "/" "\\" groupname ${res} )
      source_group( header\\${groupname} FILES ${child} )
    endif()
  endforeach()
endmacro()

macro( as_group_autogen_sources curdir lib )
	file( GLOB children ${curdir}/* )
	foreach( child ${children} )
		if( IS_DIRECTORY ${child} )
			as_group_autogen_sources( ${child} ${lib} )
		else()
			string( REPLACE ${CMAKE_BINARY_DIR}/autogen/src/${lib} "/" res ${curdir} )
			string( REPLACE "/" "\\" groupname ${res} )
      source_group( src\\${groupname} FILES ${child} )
    endif()
  endforeach()
endmacro()

macro( as_group_autogen_headers curdir lib )
	file( GLOB children ${curdir}/* )
	foreach( child ${children} )
		if( IS_DIRECTORY ${child} )
			as_group_autogen_headers( ${child} ${lib} )
		else()
			string( REPLACE ${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib} "/" res ${curdir} )
			string( REPLACE "/" "\\" groupname ${res} )
      source_group( header\\${groupname} FILES ${child} )
    endif()
  endforeach()
endmacro()