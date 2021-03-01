astateful_add_library( token )

set( h1_states 
  Method
  Uri
  Version
  Line
  HeaderKey
  HeaderVal
  HeaderNext
  Body
  Unknown
  QueryKey
  QueryVal
  QueryNext
  Check
  Done
  UnknownPercent
  QueryKeyPercent
  QueryValPercent )

as_fsm_char( h1_table G "Method" "Method" )
as_fsm_char( h1_table E "Method" "Method" )
as_fsm_char( h1_table T "Method" "Method" )
as_fsm_char( h1_table P "Method" "Method" )
as_fsm_char( h1_table O "Method" "Method" )
as_fsm_char( h1_table S "Method" "Method" )
as_fsm_char( h1_table U "Method" "Method" )
as_fsm_char( h1_table D "Method" "Method" )
as_fsm_char( h1_table L "Method" "Method" )
as_fsm_char( h1_table " " "Method" "Uri" )

as_fsm_upp( h1_table "Uri" "Uri" )
as_fsm_low( h1_table "Uri" "Uri" )
as_fsm_num( h1_table "Uri" "Uri" )
as_fsm_char( h1_table "/" "Uri" "Uri" )
as_fsm_char( h1_table "." "Uri" "Uri" )
as_fsm_char( h1_table "-" "Uri" "Uri" )
as_fsm_char( h1_table "~" "Uri" "Uri" )
as_fsm_char( h1_table " " "Uri" "Version" )
as_fsm_char( h1_table "?" "Uri" "QueryKey" )
as_fsm_char( h1_table "%" "Uri" "UnknownPercent" )

as_fsm_upp( h1_table "UnknownPercent" "UnknownPercent" )
as_fsm_low( h1_table "UnknownPercent" "UnknownPercent" )
as_fsm_num( h1_table "UnknownPercent" "UnknownPercent" )

as_fsm_upp( h1_table "QueryKey" "QueryKey" )
as_fsm_low( h1_table "QueryKey" "QueryKey" )
as_fsm_num( h1_table "QueryKey" "QueryKey" )
as_fsm_char( h1_table "=" "QueryKey" "QueryVal" )
as_fsm_char( h1_table "%" "QueryKey" "QueryKeyPercent" )

as_fsm_upp( h1_table "QueryKeyPercent" "QueryKeyPercent" )
as_fsm_low( h1_table "QueryKeyPercent" "QueryKeyPercent" )
as_fsm_num( h1_table "QueryKeyPercent" "QueryKeyPercent" )

as_fsm_upp( h1_table "QueryVal" "QueryVal" )
as_fsm_low( h1_table "QueryVal" "QueryVal" )
as_fsm_num( h1_table "QueryVal" "QueryVal" )
as_fsm_char( h1_table "_" "QueryVal" "QueryVal" )
as_fsm_char( h1_table "|" "QueryVal" "QueryVal" )
as_fsm_char( h1_table "&" "QueryVal" "QueryNext" )
as_fsm_char( h1_table " " "QueryVal" "Version" )
as_fsm_char( h1_table "%" "QueryVal" "QueryValPercent" )

as_fsm_upp( h1_table "QueryValPercent" "QueryValPercent" )
as_fsm_low( h1_table "QueryValPercent" "QueryValPercent" )
as_fsm_num( h1_table "QueryValPercent" "QueryValPercent" )

as_fsm_upp( h1_table "QueryNext" "QueryKey" )
as_fsm_low( h1_table "QueryNext" "QueryKey" )
as_fsm_num( h1_table "QueryNext" "QueryKey" )
as_fsm_char( h1_table "%" "QueryNext" "QueryKeyPercent" )

as_fsm_char( h1_table H "Version" "Version" )
as_fsm_char( h1_table T "Version" "Version" )
as_fsm_char( h1_table T "Version" "Version" )
as_fsm_char( h1_table P "Version" "Version" )
as_fsm_char( h1_table 1 "Version" "Version" )
as_fsm_char( h1_table / "Version" "Version" )
as_fsm_char( h1_table . "Version" "Version" )
as_fsm_car( h1_table "Version" "Line" )

as_fsm_new( h1_table "Line" "Unknown" )

as_fsm_car( h1_table "Unknown" "Unknown" )
as_fsm_new( h1_table "Unknown" "Check" )
as_fsm_upp( h1_table "Unknown" "HeaderKey" )
as_fsm_low( h1_table "Unknown" "HeaderKey" )

as_fsm_upp( h1_table "HeaderKey" "HeaderKey" )
as_fsm_low( h1_table "HeaderKey" "HeaderKey" )
as_fsm_char( h1_table "-" "HeaderKey" "HeaderKey" )
as_fsm_char( h1_table ":" "HeaderKey" "HeaderVal" )

as_fsm_upp( h1_table "HeaderVal" "HeaderVal" )
as_fsm_low( h1_table "HeaderVal" "HeaderVal" )
as_fsm_num( h1_table "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "." "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "-" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table " " "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "=" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "/" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "," "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "+" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "\;" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "*" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "(" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table ")" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table ":" "HeaderVal" "HeaderVal" )
as_fsm_char( h1_table "_" "HeaderVal" "HeaderVal" )
as_fsm_car( h1_table "HeaderVal" "HeaderNext" )

as_fsm_new( h1_table "HeaderNext" "Unknown" )

as_fsm_upp( h1_table "Body" "Body" )
as_fsm_low( h1_table "Body" "Body" )
as_fsm_num( h1_table "Body" "Body" )
as_fsm_char( h1_table "=" "Body" "Body" )
as_fsm_char( h1_table "_" "Body" "Body" )
as_fsm_char( h1_table "&" "Body" "Body" )

as_gen_token_fsm( h1 "${h1_states}" "${h1_table}" "Method" "Done" "uint8_t" )

set( graph_states
  Start 
  Document
  Connection
  Done )
 
as_fsm_char( graph_table "/" "Start" "Document" )
as_fsm_upp( graph_table "Document" "Document" )
as_fsm_low( graph_table "Document" "Document" )
as_fsm_num( graph_table "Document" "Document" )
as_fsm_char( graph_table "/" "Done" "Connection" )
as_fsm_upp( graph_table "Connection" "Connection" )
as_fsm_low( graph_table "Connection" "Connection" )

as_gen_token_fsm( graph "${graph_states}" "${graph_table}" "Start" "Done" "uint8_t" )

set( address_states 
  Start
  Hostname
  Port
  Done )
  
as_fsm_upp( address_table "Start" "Hostname" )
as_fsm_low( address_table "Start" "Hostname" )
as_fsm_upp( address_table "Hostname" "Hostname" )
as_fsm_low( address_table "Hostname" "Hostname" )
as_fsm_char( address_table ":" "Hostname" "Port" )
as_fsm_num( address_table "Port" "Port" )
as_fsm_num( address_table "Done" "Done" )
  
as_gen_token_fsm( address "${address_states}" "${address_table}" "Start" "Done" "wchar_t" )