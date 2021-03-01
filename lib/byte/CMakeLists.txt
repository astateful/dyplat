astateful_add_library( byte )

set( ws_states
  End
  Unknown
  Incoming
  Hint
  Length
  Mask
  Data )

as_gen_byte_fsm( ws "${ws_states}" "Incoming" "End" "uint8_t" )