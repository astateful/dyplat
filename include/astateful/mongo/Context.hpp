//! The MIT License (MIT)
//!
//! Copyright (c) 2014 Thomas Kovacs
//!
//! Permission is hereby granted, free of charge, to any person obtaining a
//! copy of this software and associated documentation files (the "Software"),
//! to deal in the Software without restriction, including without limitation
//! the rights to use, copy, modify, merge, publish, distribute, sublicense,
//! and / or sell copies of the Software, and to permit persons to whom the
//! Software is furnished to do so, subject to the following conditions :
//!
//! The above copyright notice and this permission notice shall be included in
//! all copies or substantial portions of the Software.
//!
//! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//! FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//! DEALINGS IN THE SOFTWARE.
#pragma once

#include <vector>
#include <memory>
#include <string>

#include "astateful/bson/Serialize.hpp"

namespace astateful {
namespace bson {
  enum class error_e;
  struct ObjectId;
}

namespace async {
namespace net {
namespace client {
  struct Engine;
}
}
}

namespace mongo {
  //!
  enum class update_e : int {
    UPSERT = 0x1,
    MULTI = 0x2,
    BASIC = 0x4
  };

  //!
  enum class index_e : int {
    UNIQUE = ( 1 << 0 ),
    DROP_DUPS = ( 1 << 2 ),
    BACKGROUND = ( 1 << 3 ),
    SPARSE = ( 1 << 4 )
  };

  using namespace astateful::bson;

  struct Response;
  struct Request;

  struct Context {
    //! The maximum size of a BSON document supported by the server.
    int32_t maxBsonSize;

    //!
    const Serialize writeConcern;

    //!
    bool isMaster;

    //! Construct a context from the server address and the client engine which
    //! drives the context.
    //!
    Context( const async::net::client::Engine&, const std::wstring&, error_e& );

    //!
    //!
    Context( const Context& ) = delete;
    Context& operator=( const Context& ) = delete;
    Context& operator=( Context&& ) = delete;
    Context( Context&& ) = delete;

    //!
    //!
    bool Send( Response&, const Request& ) const;

    //!
    //!
    bool Send( const Request& ) const;
  private:
    //! Used for communicating with the mongo server.
    const async::net::client::Engine& m_engine;

    //! The address of the server to make the request to.
    const std::wstring m_address;
  };

  //! Find a single document in a MongoDB server. out can be nullptr if you
  //! don't care about results. useful for commands
  //!
  bool find_one( const Context&,
                 const std::string&,
                 const Serialize&,
                 Serialize&,
                 error_e&,
                 Serialize* = nullptr );

  //! Remove a document from a MongoDB server.
  //!
  bool remove( const Context&, const std::string&, const Serialize&, error_e& );

  //! Insert a document into the MongoDB server.
  //!
  bool insert( const Context&, const std::string&, const Serialize&, error_e& );

  //! Ensure that the context is healthy by performing a round-trip to the server.
  //!
  bool ping( const Context&, Serialize&, error_e& );

  //! Drop a collection.
  //!
  bool drop_collection( const Context&,
                        const std::string&,
                        const std::string&,
                        Serialize&,
                        error_e& );

  //! Drop a database.
  //!
  bool drop_database( const Context&,
                      const std::string&,
                      Serialize&,
                      error_e& );

  //! Reset the error state for the connection.
  //!
  bool reset_error( const Context&, const std::string&, Serialize&, error_e& );

  //! Get the most recent error with the current connection.
  //!
  bool get_previous_error( const Context&,
                           const std::string&,
                           Serialize& );

  //! Get the error for the last command with the current connection.
  //!
  bool get_last_error( const Context&,
                       const std::string&,
                       Serialize&,
                       error_e& );

  //! Update a document in a MongoDB server.
  //!
  bool update( const Context&,
               const std::string&,
               const Serialize&,
               const Serialize&,
               update_e,
               error_e& );

  //! Add a database user.
  //!
  bool add_user( const Context&,
                 const std::string&,
                 const std::string&,
                 const std::string&,
                 error_e& error );

  //! Create a compound index.
  //!
  bool create_index( const Context&,
                     const std::string&,
                     const Serialize&,
                     const std::string&,
                     int,
                     int,
                     Serialize&,
                     error_e& );

  //! Create an index with a single key.
  //!
  bool create_simple_index( const Context&,
                            const std::string&,
                            const std::string&,
                            const std::string&,
                            int,
                            Serialize&,
                            error_e& );

  //! Create a capped collection.
  //!
  bool create_capped_col( const Context& context, const std::string& ns,
                          int size, int max, Serialize& result,
                          error_e& error );

  //! Return the master status of the inputted context.
  //!
  bool get_master_status( const Context& context, Serialize& result,
                          error_e& error );

  //! Insert a batch of BSON documents into a collection.
  //!
  bool insert_batch( const Context& context, const std::string& ns,
                     const std::vector<Serialize>& documents,
                     bool continue_on_failure, error_e& error );

  //! Count the number of documents in a collection matching a query.
  //!
  bool count( const Context& context, const std::string& ns, error_e& error,
              Serialize& result, const Serialize * query );

  //!
  //!
  bool md5_lookup( const Context& context, const std::string& db,
                   const ObjectId& oid, const std::string& root,
                   Serialize& result, error_e& error );
}
}
