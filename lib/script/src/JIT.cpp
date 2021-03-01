#include "astateful/script/JIT.hpp"
#include "astateful/script/Context.hpp"

#include "Cache.hpp"
#include "Memory.hpp"
#include "Lexer.hpp"
#include "Parse.hpp"

#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/DataStream.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/MemoryBuffer.h>

namespace astateful {
namespace script {
namespace {
  std::unique_ptr<llvm::Module> parse_input_ir( const llvm::MemoryBufferRef& buffer,
                                                llvm::LLVMContext& Context,
                                                const llvm::StringRef& id ) {
    llvm::SMDiagnostic diagnostic;
    auto module = llvm::parseIR( buffer, diagnostic, Context );
    if ( !module ) {
      diagnostic.print( "IR parsing failed: ", llvm::errs() );
      return nullptr;
    }

    module->setModuleIdentifier( id );

    return std::move( module );
  }

  Parse parse;
}

  JIT::JIT( Context& context ) :
    m_cache( nullptr ),
    m_context( context ) {}

  JIT::JIT( Context& context, const std::string& cache_path ) :
    m_cache( std::make_unique<Cache>( cache_path ) ),
    m_context( context ) {}

  JIT::JIT( Context& context,
            const std::string& cache_path,
            const llvm::MemoryBuffer& buffer ) :
    m_cache( std::make_unique<Cache>( cache_path ) ),
    m_context( context ) {
    llvm::SmallString<128> id = "IR:";
    id += llvm::sys::path::stem( buffer.getBufferIdentifier() );

    std::string x = id.c_str();

    module_list.push_back( parse_input_ir( buffer.getMemBufferRef(),
      m_context,
      id ) );

    current_module = module_list.back().get();
  }

  JIT::JIT( Context& context,
            const llvm::MemoryBuffer& buffer ) :
    m_cache( nullptr ),
    m_context( context ) {
    llvm::SmallString<128> id = "IR:";
    id += llvm::sys::path::stem( buffer.getBufferIdentifier() );

    std::string x = id.c_str();

    module_list.push_back( parse_input_ir( buffer.getMemBufferRef(),
      m_context,
      id ) );

    current_module = module_list.back().get();
  }

  JIT::JIT( JIT&& rhs ) :
    m_cache( std::move( rhs.m_cache ) ), 
    m_context( std::move( rhs.m_context ) ),
    module_list( std::move( rhs.module_list ) ),
    m_engine( std::move( rhs.m_engine ) ),
    current_module( std::move( rhs.current_module ) ) {}

  llvm::Function * JIT::getFunction( const std::string FnName )
  {
    for ( auto it = module_list.begin(); it !=  module_list.end(); ++it )
    {
      llvm::Function *F = ( *it )->getFunction( FnName );
      if ( F )
      {
        if ( ( *it ).get() == current_module )
        {
          return F;
        }

        assert( current_module != nullptr );

        // This function is in a module that has already been JITed.
        // We just need a prototype for external linkage.
        llvm::Function *PF = current_module->getFunction( FnName );
        if ( PF && !PF->empty() )
        {
          //ErrorF ("redefinition of function across modules");
          return 0;
        }

        // If we don't have a prototype yet, create one.
        if ( !PF )
        {
          PF = llvm::Function::Create( F->getFunctionType(),
                                        llvm::Function::ExternalLinkage,
                                        FnName,
                                        current_module );
        }

        return PF;
      }
    }

    return nullptr;
  }

  std::string JIT::GenerateUniqueName( const std::string& root ) {
    static int i = 0;
    return root + std::to_string( i++ );
  }

  bool JIT::output( llvm::raw_ostream& stream ) const {
    if ( !current_module ) return false;

    stream << *current_module;

    return true;
  }

  bool JIT::ModuleName( std::string& name ) const {
    if ( !current_module ) return false;

    name = std::string( current_module->getModuleIdentifier().c_str() );

    return true;
  }

  llvm::Module * JIT::getModuleForNewFunction()
  {
    // If we have a Module that hasn't been JITed, use that.
    if ( current_module != nullptr ) return current_module;

    std::string ModName = GenerateUniqueName ( "mcjit_module_" );
    module_list.push_back ( std::make_unique<llvm::Module>( ModName, m_context ) );

    current_module = module_list.back().get();

    return current_module;
  }

  llvm::ExecutionEngine * JIT::compileModule( std::unique_ptr<llvm::Module>& M_input )
  {
    assert( m_engine.find ( M_input.get() ) == m_engine.end () );

    llvm::Module * M = M_input.get();

    if ( M == current_module ) { closeCurrentModule (); }

    std::string ErrStr;
    llvm::ExecutionEngine *EE = llvm::EngineBuilder( std::move( M_input ) )
                                .setErrorStr ( &ErrStr )
                                .setMCJITMemoryManager ( std::make_unique<Memory>( *this ) )
                                .setEngineKind( llvm::EngineKind::JIT )
                                .create ();

    if ( !EE )
    {
      fprintf ( stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str () );
      exit ( 1 );
    }

    if ( m_cache ) { EE->setObjectCache( m_cache.get() ); }

    // Get the ModuleID so we can identify IR input files
    const std::string ModuleID = M->getModuleIdentifier();

    // If we've flagged this as an IR file, it doesn't need function passes run.
    if ( 0 != ModuleID.compare ( 0, 3, "IR:" ) )
    {
      llvm::legacy::FunctionPassManager FPM( M );

      // Set up the optimizer pipeline.  Start with registering info about how the
      // target lays out data structures.
      M->setDataLayout( EE->getDataLayout() );

      // Provide basic AliasAnalysis support for GVN.
      FPM.add( llvm::createBasicAliasAnalysisPass() );
      // Promote allocas to registers.
      FPM.add( llvm::createPromoteMemoryToRegisterPass() );
      // Do simple "peephole" optimizations and bit-twiddling optzns.
      FPM.add( llvm::createInstructionCombiningPass() );
      // Reassociate expressions.
      FPM.add( llvm::createReassociatePass() );
      // Eliminate Common SubExpressions.
      FPM.add( llvm::createGVNPass() );
      // Simplify the control flow graph (deleting unreachable blocks, etc).
      FPM.add( llvm::createCFGSimplificationPass() );

      FPM.doInitialization();

      // For each function in the module
      for ( auto it = M->begin(); it != M->end(); ++it )
      {
        // Run the FPM on this function
        FPM.run( *it );
      }
    }

    EE->finalizeObject();

    m_engine[M] = EE;

    return EE;
  }

  void * JIT::getPointerToFunction( llvm::Function* F )
  {
    std::string FnName = F->getName();

    for ( auto it = module_list.begin(); it != module_list.end(); ++it )
    {
      llvm::Function *MF = ( *it )->getFunction( FnName );
      if ( MF == F )
      {
        auto eeIt = m_engine.find( ( *it ).get() );
        if ( eeIt != m_engine.end() )
        {
          void *P = eeIt->second->getPointerToFunction( F );
          if ( P )
          {
            return P;
          }
        }
        else
        {
          llvm::ExecutionEngine *EE = compileModule( *it );
          void *P = EE->getPointerToFunction( F );
          if ( P )
          {
            return P;
          }
        }
      }
    }

    return nullptr;
  }

  void JIT::closeCurrentModule()
  {
    // If we have an open module (and we should), pack it up
    if ( current_module )
    {
      current_module = nullptr;
    }
  }

  void * JIT::getPointerToNamedFunction( const std::string& Name )
  {
    for ( auto it = module_list.begin(); it != module_list.end(); ++it )
    {
      if ( !*it ) { continue; }

      llvm::Function *F = ( *it )->getFunction( Name );
      if ( F && !F->empty() )
      {
        auto eeIt = m_engine.find( ( *it ).get() );
        if ( eeIt != m_engine.end() )
        {
          void *P = eeIt->second->getPointerToFunction( F );
          if ( P )
          {
            return P;
          }
        }
        else
        {
          llvm::ExecutionEngine *EE = compileModule ( *it );
          void *P = EE->getPointerToFunction ( F );
          if ( P ) return P;
        }
      }
    }

    return nullptr;
  }

  JIT::~JIT() {
    for ( auto& it : module_list ) {
      // See if we have an execution engine for this module.
      auto mapIt = m_engine.find( it.get() );

      // If we have an EE, the EE owns the module so just delete the EE.
      if ( mapIt != m_engine.end() ) {
        delete mapIt->second;
      } else {
        // Otherwise, we still own the module.  Delete it now.
        it.release();
      }
    }
  }

  JIT generate_jit( Context& context,
                    const std::string& file_name,
                    const std::string& cache_path,
                    const std::string& source_path ) {
    // If the IR exists, create a JIT with the IR file.
    llvm::SmallString<256> ir_file = cache_path;
    llvm::sys::path::append( ir_file, file_name + ".ir" );

    llvm::Twine twine_ir_file = ir_file;
    if ( llvm::sys::fs::exists( twine_ir_file ) ) {
      auto buffer = llvm::MemoryBuffer::getFile( ir_file.c_str() );
      return { context, cache_path, *buffer.get() };
    }

    // If the IR does not exist, we need to parse the source file.
    llvm::SmallString<256> as_file = source_path;
    llvm::sys::path::append( as_file, file_name + ".as" );

    // Check to see if the source file exists (need a Twine!?)
    llvm::Twine twine_as_file = as_file;
    if ( !llvm::sys::fs::exists( twine_as_file ) )
      return { context, cache_path };

    // Populate the JIT as the source file is parsed.
    auto buffer = llvm::MemoryBuffer::getFile( twine_as_file );

    JIT jit( context, cache_path );
    Lexer lexer( *buffer.get() );
    //Parse parse;
    parse( jit, context, lexer );

    // Create the cache directory if it does not already exist. In
    // this directory the IR and BC files will be generated.
    llvm::Twine twine_cache_path = cache_path;
    auto error = llvm::sys::fs::create_directory( twine_cache_path, true );
    if ( error ) return { context };

    // Output the IR from the JIT to the cache path.
    std::error_code error_code;
    llvm::raw_fd_ostream stream( ir_file,
                                 error_code,
                                 llvm::sys::fs::OpenFlags::F_RW );

    if ( !jit.output( stream ) ) return { context };

    return jit;
  }
}
}
