astateful_add_example( CompleteServer ssl )

target_link_libraries( CompleteServer
	async
	BSON
	byte
	crypto
	mongo
	plugin
	protocol
	script
	SSL
	token )

target_link_libraries( CompleteServer ws2_32.lib Shlwapi.lib )
find_package( LLVM REQUIRED CONFIG )
add_definitions( ${LLVM_DEFINITIONS} )
llvm_map_components_to_libnames( llvm_libs all )
target_link_libraries( CompleteServer ${llvm_libs} )
