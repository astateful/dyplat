#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/bson/Iterate.hpp"
#include "astateful/bson/Serialize.hpp"
#include "astateful/bson/Element/Binary/Function.hpp"
#include "astateful/bson/Element/Binary/Generic.hpp"
#include "astateful/bson/Element/Binary/MD5.hpp"
#include "astateful/bson/Element/Binary/UDEF.hpp"
#include "astateful/bson/Element/Binary/UUID.hpp"
#include "astateful/bson/Element/Array.hpp"
#include "astateful/bson/Element/Bool.hpp"
#include "astateful/bson/Element/Code.hpp"
#include "astateful/bson/Element/Codescope.hpp"
#include "astateful/bson/Element/Datetime.hpp"
#include "astateful/bson/Element/Double.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/bson/Element/Long.hpp"
#include "astateful/bson/Element/Null.hpp"
#include "astateful/bson/Element/Object.hpp"
#include "astateful/bson/Element/ObjectId.hpp"
#include "astateful/bson/Element/Regex.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Timestamp.hpp"
#include "astateful/bson/JSON.hpp"
#include "astateful/bson/Error.hpp"

using namespace astateful;
using namespace astateful::bson;

TEST_CASE( "static element members" ) {
  auto error = error_e::CLEAN;

  SECTION( "validate string" ) {
    REQUIRE( Element::validateString( error, "nothing", false, false ) );
    REQUIRE( error == error_e::CLEAN );

    REQUIRE( Element::validateString( error, "$nothing", false, true ) );
    REQUIRE( error == error_e::FIELD_INIT_DOLLAR );

    error = error_e::CLEAN;
    REQUIRE( Element::validateString( error, "noth.ing", true, false ) );
    REQUIRE( error == error_e::FIELD_HAS_DOT );

    error = error_e::CLEAN;
    REQUIRE( Element::validateString( error, "$no.thing", true, true ) );
    REQUIRE( ( error & error_e::FIELD_HAS_DOT ) == error_e::FIELD_HAS_DOT );
    REQUIRE( ( error & error_e::FIELD_INIT_DOLLAR ) == error_e::FIELD_INIT_DOLLAR );

    error = error_e::CLEAN;
    REQUIRE( !Element::validateString( error, "\xa0\xa1", true, true ) );
    REQUIRE( error == error_e::NOT_UTF8 );
  }

  SECTION( "escape string" ) {
    REQUIRE( Element::escapeJSON( L"\xef\xef\x26" ) == L"\\u00ef\\u00ef&" );
  }

  SECTION( "append int") {
    int32_t value = 718272;
    std::vector<uint8_t> output;
    REQUIRE( Element::appendInt( output, value ) );
    REQUIRE ( output[0] == 192 );
    REQUIRE ( output[1] == 245 );
    REQUIRE ( output[2] == 10 );
    REQUIRE ( output[3] == 0 );
  }

  SECTION( "append string" ) {
    std::vector<uint8_t> output;
    const std::string value = "value";
    REQUIRE( Element::appendString( error, output, value ) );
    REQUIRE( error == error_e::CLEAN );

    std::vector<uint8_t> check;
    REQUIRE( Element::appendInt( check, value.size() + 1 ) );
    check.insert( check.end(), value.begin(), value.end() );
    check.push_back( '\0' );

    REQUIRE( output == check );
  }
}

TEST_CASE( "serialize & element" ) {
  auto error = error_e::CLEAN;

  Serialize test;
  test.append( ElementString( "string", "value" ) );
  test.append( ElementInt( "int", 10000 ) );
  test.append( ElementLong( "long", 21474836 ) );
  test.append( ElementDatetime( "datetime", 130105669019377920LL ) );
  test.append( ElementDouble( "double", .56789 ) );
  test.append( ElementRegex( "regex", L"^asdf", L"\\i\u00c6mxp" ) );
  test.append( ElementCode( "js", L"function() { var x = \"test\"; return x; };" ) );
  test.append( ElementTimestamp( "timestamp", 384854754, 1294353 ) );
  test.append( ElementBool( "bool_true", true ) );
  test.append( ElementBool( "bool_false", false ) );
  test.append( ElementObjectId( "$id" ) );
  test.append( ElementObjectId( "$ref", "507f191e810c19729de860ea" ) );
  test.append( ElementNull( "null" ) );

  SECTION( "codescope" ) {
    Serialize scope;
    scope.append( ElementInt( "i", 123 ) );
    scope.append( ElementCode( "test", L"function() { return j; }" ) );
    scope.append( ElementString( "k", L"testing" ) );

    test.append( ElementCodescope( "test", L"function() { return k; }",
      scope,
      error ) );

    REQUIRE( error == error_e::CLEAN );
  }

  SECTION( "binary" ) {
    Serialize binary;
    binary.append( ElementBinaryGeneric( "generic", "beef" ) );
    binary.append( ElementBinaryFunction( "function", "beef1" ) );
    binary.append( ElementBinaryMD5( "md5", "7815696ecbf1c96e6894b779456d330e" ) );
    binary.append( ElementBinaryUUID( "uuid", "14CFAEA0-422E-11E2-A25F-0800200C9A66" ) );
    binary.append( ElementBinaryUDEF( "udef", "beef1" ) );

    REQUIRE( test.append( "binary", binary, error ) );
    REQUIRE( error == error_e::CLEAN );
  }

  SECTION( "datetime" ) {
    ElementDatetime dt( "dt", 1428239318000 );
    REQUIRE( std::string( dt ) == "2015-04-05T13:08:38Z" );
  }

  std::vector<uint8_t> bson;
  REQUIRE( test.bson( bson, error ) );
  REQUIRE( error == error_e::CLEAN );

  Serialize check( bson );
  REQUIRE( std::string( check["string"] ) == "value" );
  REQUIRE( int32_t( check["int"] ) == 10000 );
  REQUIRE( int64_t( check["long"] ) == 21474836 );
  REQUIRE( int64_t( check["datetime"] ) == 130105669019377920LL );
  REQUIRE( double( check["double"] ) == .56789 );
  REQUIRE( bool( check["bool_true"] ) == true );
  REQUIRE( bool( check["bool_false"] ) == false );
}

TEST_CASE( "json & iterate" ) {
  auto error = error_e::CLEAN;

  Serialize result;

  SECTION( "create a bson from json" ) {
    std::string input = R"({
      "root": {
        "string":"string value",
        "true":true,
        "sub_doc": {
          "empty": null,
          "false":false,
          "array": {
            "basic":[1,2,3,4],
            "matrix":[[1,2],[3,4]],
            "mixed":[{"key":[{"a":"b"},1,"2",3.0],"test":false},123,"abc"]
          },
          "double":.569
        },
        "quoted string":"string \"quoted\"",
        "integer":123
      }
    })";

    result = bson::json::convert( input, error );
    REQUIRE( error == error_e::CLEAN );
  }

  Iterate def( result, error );
  REQUIRE( error == error_e::CLEAN );

  while ( def ) {
    if ( def->key() == "root" ) {
      REQUIRE( def.enumId() == element_e::OBJECT );

      Iterate root( def->bson( error ) );
      REQUIRE( error == error_e::CLEAN );
      while ( root ) {
        if ( root->key() == "string" ) {
          REQUIRE( root.enumId() == element_e::STRING );
          REQUIRE( std::string( *root ) == "string value" );
        } else if ( root->key() == "quoted string" ) {
          REQUIRE( root.enumId() == element_e::STRING );
          REQUIRE( std::string( *root ) == "string \\\"quoted\\\"" );
        } else if ( root->key() == "integer" ) {
          REQUIRE( root.enumId() == element_e::INT );
          REQUIRE( int32_t( *root ) == 123 );
        } else if ( root->key() == "true" ) {
          REQUIRE( root.enumId() == element_e::BOOL );
          REQUIRE( bool( *root ) == true );
        } else if ( root->key() == "sub_doc" ) {
          REQUIRE( root.enumId() == element_e::OBJECT );

          Iterate sub_doc( root->bson( error) );
          REQUIRE( error == error_e::CLEAN );
          while ( sub_doc ) {
            if ( sub_doc->key() == "empty" ) {
              REQUIRE( sub_doc.enumId() == element_e::EMPTY );
            } else if ( sub_doc->key() == "false" ) {
              REQUIRE( sub_doc.enumId() == element_e::BOOL );
              REQUIRE( bool( *sub_doc ) == false );
            } else if ( sub_doc->key() == "double" ) {
              REQUIRE( sub_doc.enumId() == element_e::DOUBLE );
              REQUIRE( double( *sub_doc ) == .569 );
            } else if ( sub_doc->key() == "array" ) {
              REQUIRE( sub_doc.enumId() == element_e::OBJECT );

              Iterate array( sub_doc->bson( error ) );
              REQUIRE( error == error_e::CLEAN );
              while ( array ) {
                if ( array->key() == "basic" ) {
                  REQUIRE( array.enumId() == element_e::ARRAY );

                  Iterate basic( array->bson( error) );
                  REQUIRE( error == error_e::CLEAN );
                  int inc = 0;
                  while ( basic ) {
                    REQUIRE(basic->key() == std::to_string( inc ) );
                    REQUIRE( int32_t( *basic ) == ( inc + 1 ) );
                    ++basic;
                    ++inc;
                  }
                } else if ( array->key() == "matrix" ) {
                  REQUIRE( array.enumId() == element_e::ARRAY );

                  Iterate matrix( array->bson( error) );
                  REQUIRE( error == error_e::CLEAN );
                  int inc = 0;
                  while ( matrix ) {
                    Iterate row( matrix->bson( error ) );
                    REQUIRE( error == error_e::CLEAN );
                    while ( row ) {
                      REQUIRE( int32_t( *row ) == ( inc + 1 ) );
                      ++inc;
                      ++row;
                    }

                    ++matrix;
                  }
                } else if ( array->key() == "mixed" ) {
                  REQUIRE( array.enumId() == element_e::ARRAY );

                  Iterate mixed( array->bson( error ) );
                  REQUIRE( error == error_e::CLEAN );
                  REQUIRE( mixed->key() == "0" );
                  REQUIRE( mixed.enumId() == element_e::OBJECT );

                  Iterate inner( mixed->bson(error) );
                  REQUIRE( error == error_e::CLEAN );
                  REQUIRE( inner->key() == "key" );

                  Iterate sub_inner( inner->bson( error ) );
                  REQUIRE( error == error_e::CLEAN );
                  REQUIRE( sub_inner->key() == "0" );

                  Iterate sub_sub_inner( sub_inner->bson( error ) );
                  REQUIRE( error == error_e::CLEAN );
                  REQUIRE( sub_sub_inner );
                  REQUIRE( sub_sub_inner->key() == "a" );
                  REQUIRE( std::string( *sub_sub_inner ) == "b" );

                  ++sub_inner;
                  REQUIRE( sub_inner->key() == "1" );
                  REQUIRE( int32_t( *sub_inner ) == 1 );
                  ++sub_inner;
                  REQUIRE( sub_inner->key() == "2" );
                  REQUIRE( std::string( *sub_inner ) == "2" );
                  ++sub_inner;
                  REQUIRE( sub_inner->key() == "3" );
                  REQUIRE( double( *sub_inner ) == 3.0 );
                  ++inner;
                  REQUIRE( inner->key() == "test" );
                  REQUIRE( bool( *inner ) == false );
                  ++mixed;
                  REQUIRE( mixed->key() == "1" );
                  REQUIRE( int32_t( *mixed ) == 123 );
                  ++mixed;
                  REQUIRE( mixed->key() == "2" );
                  REQUIRE( std::string( *mixed ) == "abc" );
                }

                ++array;
              }
            }

            ++sub_doc;
          }
        }

        ++root;
      }
    }

    ++def;
  }
}

TEST_CASE( "object id" ) {
  SECTION( "verify string construction" ) {
    ObjectId id;
    std::string str = id;
    ObjectId chk_id( str );
    std::string check_str = chk_id;
    REQUIRE( str == check_str );
  }
}