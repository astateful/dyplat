macro( as_fsm_int table int current_state next_state )
    list( APPEND ${table} "\t\t{ state_e::${current_state},\t${int},\tstate_e::${next_state} },\n" )
endmacro()

macro( as_fsm_char table char current_state next_state )
    list( APPEND ${table} "\t\t{ state_e::${current_state},\t'${char}',\tstate_e::${next_state} },\n" )
endmacro()

macro( as_fsm_new table current_state next_state )
    list( APPEND ${table} "\t\t{ state_e::${current_state},\t10,\tstate_e::${next_state} },\n" )
endmacro()

macro( as_fsm_car table current_state next_state )
    list( APPEND ${table} "\t\t{ state_e::${current_state},\t13,\tstate_e::${next_state} },\n" )
endmacro()

macro( as_fsm_upp table current_state next_state )
  foreach( int_var RANGE 65 90 )
    as_fsm_int( ${table} ${int_var} ${current_state} ${next_state} )
  endforeach()
endmacro()

macro( as_fsm_low table current_state next_state )
  foreach( int_var RANGE 97 122 )
    as_fsm_int( ${table} ${int_var} ${current_state} ${next_state} )
  endforeach()
endmacro()

macro( as_fsm_num table current_state next_state )
  foreach( int_var RANGE 48 57 )
    as_fsm_int( ${table} ${int_var} ${current_state} ${next_state} )
  endforeach()
endmacro()

macro( as_gen_fsm_state_derived_headers lib ns states type )
  cmake_policy( SET CMP0053 NEW )
  foreach( state ${states} )
    file( REMOVE ${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib}/${ns}/State/${state}.hpp )
    file( APPEND ${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib}/${ns}/State/${state}.hpp
    "//! AUTOGEN
#pragma once
    
#include \"../State.hpp\"

namespace astateful {
namespace ${lib} {
namespace ${ns} {
  //!
  struct State${state} : ${lib}::${ns}::State {
    //!
    state_e operator()( Context&, ${type} );
  };
}
}
}" )
  endforeach()
endmacro()

macro( as_gen_fsm_state_base_header lib ns states type )
  SET( enums "")
  foreach( state ${states} )
    SET( enums "\t\t${state},\n" ${enums} )
  endforeach()
  
  string( REPLACE ";" "" enums ${enums})

  file( REMOVE ${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib}/${ns}/State.hpp )
  file( APPEND ${CMAKE_BINARY_DIR}/autogen/include/autogen/${lib}/${ns}/State.hpp
"//! AUTOGEN
#pragma once
  
#include \"astateful/${lib}/State.hpp\"

namespace astateful {
namespace ${lib} {
namespace ${ns} {
  //!
  enum class state_e {
${enums}
  };
    
  //!
  struct Context;

  //!
  struct State : public ${lib}::State<state_e, Context, ${type}> {};
}
}
}" )
endmacro()

macro( as_gen_fsm_token_table ns states table initial_state success_state type )
  set( filename ${CMAKE_BINARY_DIR}/autogen/src/token/${ns}/Machine.cpp )
  
  file( REMOVE ${filename} )
  file( APPEND ${filename} "#include \"astateful/token/${ns}/Machine.hpp\"\n\n" )
  
  # Need the state class definitions.
  foreach( state ${states} )
    file( APPEND ${filename} "#include \"autogen/token/${ns}/State/${state}.hpp\"\n" )
  endforeach()
  
  file( APPEND ${filename} "
namespace astateful {
namespace token {
namespace ${ns} {
  Machine::Machine() {\n" )
  
  # Generate a class representing each state. For each new transition
  # the accompanying state will be called to produce the next transition.
  foreach( state ${states} )
    file( APPEND ${filename} "\t\tm_state[${ns}::state_e::${state}] = std::make_unique<${ns}::State${state}>();\n" )
  endforeach()
  
  # Generate the static transition table powering the FSM as well
  # as the accompanying accessor methods that the FSM hook into.
  file( APPEND ${filename} "\t}
  
  static const token::Table<state_e, ${type}> my_table[] = {\n" )
  foreach( t ${table})
    file( APPEND ${filename} "${t}" )
  endforeach()
  file( APPEND ${filename} "\t};

  static const unsigned int TABLE_SIZE = sizeof( my_table ) / sizeof( my_table[0] );
      
  const token::Table<state_e, ${type}> * Machine::beginTable() const {
    return &my_table[0];
  }
  
  const token::Table<state_e, ${type}> * Machine::endTable() const {
    return &my_table[TABLE_SIZE];
  }
  
  state_e Machine::initialState() const {
    return state_e::${initial_state};
  }
  
  state_e Machine::successState() const {
    return state_e::${success_state};
  }
}
}
}" )
endmacro()

macro( as_gen_fsm_byte_table ns states initial_state success_state type )
  set( filename ${CMAKE_BINARY_DIR}/autogen/src/byte/${ns}/Machine.cpp )
  
  file( REMOVE ${filename} )
  file( APPEND ${filename} "#include \"astateful/byte/${ns}/Machine.hpp\"\n\n" )
  
  # Need the state class definitions.
  foreach( state ${states} )
    file( APPEND ${filename} "#include \"autogen/byte/${ns}/State/${state}.hpp\"\n" )
  endforeach()
  
  file( APPEND ${filename} "
namespace astateful {
namespace byte {
namespace ${ns} {
  Machine::Machine() {\n" )
  
  # Generate a class representing each state. For each new transition
  # the accompanying state will be called to produce the next transition.
  foreach( state ${states} )
    file( APPEND ${filename} "\t\tm_state[${ns}::state_e::${state}] = std::make_unique<${ns}::State${state}>();\n" )
  endforeach()
  
  file( APPEND ${filename} "\t}
  
  state_e Machine::initialState() const {
    return state_e::${initial_state};
  }
  
  state_e Machine::successState() const {
    return state_e::${success_state};
  }
}
}
}" )
endmacro()

macro( as_gen_token_fsm ns states table initial_state success_state type )
  as_gen_fsm_state_base_header( token ${ns} "${states}" ${type} )
  as_gen_fsm_state_derived_headers( token ${ns} "${states}" ${type} )
  as_gen_fsm_token_table( ${ns} "${states}" "${table}" ${initial_state} ${success_state} ${type}  )
endmacro()

macro( as_gen_byte_fsm ns states initial_state success_state type )
  as_gen_fsm_state_base_header( byte ${ns} "${states}" ${type} )
  as_gen_fsm_state_derived_headers( byte ${ns} "${states}" ${type} )
  as_gen_fsm_byte_table( ${ns} "${states}" ${initial_state} ${success_state} ${type}  )
endmacro()