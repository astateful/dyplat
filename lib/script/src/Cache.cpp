#include "Cache.hpp"

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>

namespace astateful {
namespace script {
  Cache::Cache( const std::string& path ) : m_path( path ) {}

  void Cache::notifyObjectCompiled( const llvm::Module * module,
                                    llvm::MemoryBufferRef buffer ) {
    const auto module_id = module->getModuleIdentifier ();

    if ( 0 == module_id.compare( 0, 3, "IR:" ) ) {
      auto ir_filename = module_id.substr( 3 );
      ir_filename += ".bc";

      llvm::SmallString<128> cache_file = m_path;
      llvm::sys::path::append( cache_file, ir_filename );

      std::error_code ErrStr;
      llvm::StringRef ref( cache_file.c_str() );
      llvm::raw_fd_ostream object_file( ref, ErrStr, llvm::sys::fs::OpenFlags::F_RW );
      object_file << buffer.getBuffer();
    }
  }

  std::unique_ptr<llvm::MemoryBuffer> Cache::getObject( const llvm::Module* module ) {
    const auto module_id = module->getModuleIdentifier();

    if ( 0 == module_id.compare( 0, 3, "IR:" ) ) {
      auto ir_filename = module_id.substr( 3 );
      ir_filename += ".bc";

      llvm::SmallString<128> cache_file = m_path;
      llvm::sys::path::append( cache_file, ir_filename );

      if ( !llvm::sys::fs::exists( cache_file.str () ) )
        return nullptr;

      auto buffer = llvm::MemoryBuffer::getFile( cache_file.c_str () );

      return llvm::MemoryBuffer::getMemBufferCopy( ( *buffer ).get()->getBuffer() );
    }

    return nullptr;
  }
}
}
