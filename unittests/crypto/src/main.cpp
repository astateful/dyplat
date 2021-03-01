#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/crypto/base64.hpp"
#include "astateful/crypto/md5.hpp"
#include "astateful/crypto/scrypt/Context.hpp"
#include "astateful/crypto/hmac/Context.hpp"

using namespace astateful;
using namespace astateful::crypto;

TEST_CASE( "base64" ) {
  SECTION( "encoding" ) {
    REQUIRE( base64encode( "testingtesting123" ) == "dGVzdGluZ3Rlc3RpbmcxMjM=" );
    REQUIRE( base64encode( "" ) == "===" );
  }
}

TEST_CASE( "md5" ) {
  REQUIRE( md5("plaintext") == "f2bc5b1d869870d7688f71b2d87030bd" );
  REQUIRE( md5( "" ) == "d41d8cd98f00b204e9800998ecf8427e" );
}

TEST_CASE( "scrypt" ) {
  const std::string password = "password";
  const std::string data = "this is a test. this is another tests!!!";

  std::vector<uint8_t> plain( data.begin(), data.end() );
  std::vector<uint8_t> encrypted;
  std::vector<uint8_t> decrypted;

  const scrypt::Context enc_context( 0, 0.5, 300.0, password );
  const scrypt::Context dec_context( 0, 0.5, 1500.0, password );

  REQUIRE( encrypt( enc_context, plain, encrypted ) == scrypt::result_e::SUCCESS );
  REQUIRE( decrypt( dec_context, encrypted, decrypted ) == scrypt::result_e::SUCCESS );
  REQUIRE( decrypted == plain );
}
