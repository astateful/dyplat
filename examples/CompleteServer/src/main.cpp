#include "autogen/config.h"
#include "astateful/algorithm/Generate/Thread.hpp"
#include "astateful/algorithm/Parse.hpp"
#include "astateful/async/net/client/Engine.hpp"
#include "astateful/async/net/client/Connection.hpp"
#include "astateful/async/net/server/Engine.hpp"
#include "astateful/async/pipe/client/Engine.hpp"
#include "astateful/async/pipe/server/Engine.hpp"
#include "astateful/bson/Error.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element.hpp"
#include "astateful/plugin/Store/Mongo.hpp"
#include "astateful/plugin/Pipe/Scrypt.hpp"
#include "astateful/plugin/Compress.hpp"
#include "astateful/plugin/Expand.hpp"
#include "astateful/plugin/Key.hpp"
#include "astateful/script/Init.hpp"
#include "astateful/protocol/Connection.hpp"
#include "astateful/ssl/server/Connection.hpp"
#include "astateful/ssl/server/Context.hpp"
#include "astateful/ssl/Init.hpp"
#include "astateful/mongo/Context.hpp"

#include <mutex>
#include <fstream>

using namespace astateful;
using namespace astateful::async;
using namespace astateful::plugin;
using namespace astateful::protocol;

template<typename U, typename V> void run() {
  //const std::wstring pkey( install_prefix() + L"/ssl/ec/prime256v1/key.pem" );
  //const std::wstring cert( install_prefix() + L"/ssl/ec/prime256v1/cert.pem" );
  const std::wstring pkey( install_prefix() + L"/ssl/rsa/key.pem" );
  const std::wstring cert( install_prefix() + L"/ssl/rsa/cert.pem" );
  const std::wstring log_file_path( install_prefix() + L"/log.txt" );
  const std::wstring memoize_path( install_prefix() + L"/memoize" );
  const std::wstring public_html( install_prefix() + L"/public_html" );
  const std::wstring script_path( install_prefix() + L"/script" );
  const std::wstring memoize( L"memoize" );

  std::mutex mutex;
  std::unique_lock<std::mutex> lock( mutex, std::defer_lock );

  pipe::server::callback_t callback;
  callback["log"] = [&]( const std::vector<uint8_t>& buffer ) {
    lock.lock();
    std::ofstream stream( log_file_path, std::ios::out | std::ios::app );
    if ( stream.good() ) {
      stream.write( reinterpret_cast<const char *>( buffer.data() ) + 1, buffer.size() - 1 );
    }
    lock.unlock();
  };

  pipe::server::Engine pipe_server( callback );
  pipe::client::Engine pipe_client;

  pipe_server.start();
  pipe_client.start();

  net::client::connection_t client_conn;
  client_conn[L"localhost:11211"] = [&]( net::client::Response * response,
                                         const net::client::Request& request ) {
    return new net::client::Connection( response, request );
  };

  client_conn[L"localhost:27017"] = [&]( net::client::Response * response,
                                         const net::client::Request& request ) {
    return new net::client::Connection( response, request );
  };

  net::client::Engine net_client( pipe_client, client_conn );
  net_client.runIPv4();

  auto error = bson::error_e::CLEAN;

  mongo::Context mongo( net_client, L"localhost:27017", error );
  const StoreMongo store( mongo, "astateful.memoization" );
  const PipeScrypt<U> pipe( "password", store );
  const algorithm::Persist<U, V> persist;
  const algorithm::Parse<U, V> parse( persist );
  const algorithm::GenerateThread<U, V> generate( persist );

  ssl::Init ssl_init;
  ssl::server::Context ssl_context( cert, pkey, "h2-14" );
  net::server::connection_t serv_conn;

  script::Init script_init;

  Context<U,V> protocol( memoize_path,
                         public_html,
                         script_path,
                         mongo,
                         parse,
                         generate,
                         pipe );

  serv_conn[L"localhost:80"] = [&]( const net::Address& local,
                                    const net::Address& remote ) {
    return new protocol::Connection<U, V>( protocol, local, remote, "h1" );
  };

  serv_conn[L"localhost:443"] = [&]( const net::Address& local,
                                     const net::Address& remote ) {
    return new ssl::server::Connection<U, V>( protocol,
                                              local,
                                              remote,
                                              ssl_context );
  };

  net::server::Engine web_server( pipe_client, serv_conn );
  web_server.runIPv6();
}

int wmain( int argc, wchar_t * argv[] ) {
  run<bson::Serialize, std::string>();
};
