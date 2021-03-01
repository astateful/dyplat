#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/byte/h2/hpack/DynamicTable.hpp"
#include "astateful/byte/h2/hpack/HeaderField.hpp"

using namespace astateful;
using namespace astateful::byte;

TEST_CASE( "h2" ) {
  SECTION( "hpack" ) {
    h2::hpack::DynamicTable dyn_table( 102 );

    dyn_table.add( { "a", "b" } );
    dyn_table.add( { "c", "d" } );
    dyn_table.add( { "e", "f" } );

    dyn_table.add( { "g", "h" } );

    auto x = dyn_table.get(1);

    dyn_table.show();

    dyn_table.add( { "g", "h" } );

    dyn_table.show();
  }


}