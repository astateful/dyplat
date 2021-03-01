#include "Memory.hpp"

#include "astateful/script/JIT.hpp"

namespace astateful {
namespace script {
  Memory::Memory( JIT& jit ) : m_jit( jit ) {}

  uint64_t Memory::getSymbolAddress( const std::string& Name ) {
    auto Addr = ( uint64_t ) m_jit.getPointerToNamedFunction( Name );
    if ( Addr ) return static_cast<uint64_t>( Addr );

    return RTDyldMemoryManager::getSymbolAddress( Name );
  }

  void * Memory::getPointerToNamedFunction( const std::string& Name, bool AbortOnFailure ) {
    uint64_t Addr = getSymbolAddress( Name );
    // Try the standard symbol resolution first, but ask it not to abort.
    /*void *pfn = RTDyldMemoryManager::getPointerToNamedFunction ( Name, false );
    if ( pfn )
    {
    return pfn;
    }

    pfn = mpJIT->getPointerToNamedFunction ( Name );*/
    if ( !Addr && AbortOnFailure )
      llvm::report_fatal_error( "Program used external function test '" + Name +
      "' which could not be resolved!" );
    return ( void* ) Addr;
  }
}
}