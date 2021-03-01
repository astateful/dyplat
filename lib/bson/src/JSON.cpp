#include "astateful/bson/JSON.hpp"
#include "astateful/bson/Element/String.hpp"
#include "astateful/bson/Element/Int.hpp"
#include "astateful/bson/Element/Null.hpp"
#include "astateful/bson/Element/Bool.hpp"
#include "astateful/bson/Element/Double.hpp"
#include "astateful/bson/Serialize.hpp"

#include <locale>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace astateful {
namespace bson {
namespace json {
namespace {
  enum class state_e {
    UNKNOWN,
    KEY,
    SPLIT,
    VAL_UNKNOWN,
    VAL_STRING,
    VAL_ARRAY,
    VAL_OBJECT
  };

  bool is_integer( const std::string& input ) {
    if ( input.empty() ) return false;

    const auto& first = input[0];
    if ( ( !isdigit( first ) ) && ( first != '-') && ( first != '+' ) )
      return false;

    char * test_conv;
    strtol( input.c_str(), &test_conv, 10 );

    return ( *test_conv == 0 );
  }

  bool is_double( const std::string& input ) {
    char* test_conv  = 0;
    strtod( input.c_str(), &test_conv );

    if ( *test_conv  != '\0' || test_conv == input.c_str() )
      return false;

    return true;
  }

  void store_value( bool is_array, Serialize& serialize, int& array_key,
                    std::string& key, std::string& value, state_e& state ) {
    if ( is_integer( value ) ) {
      if (is_array) {
        serialize.append( ElementInt( std::to_string( array_key ), value ) );
        value = "";
        ++array_key;
        state = state_e::VAL_UNKNOWN;
      } else {
        serialize.append( ElementInt( key, value ) );
        value = "";
        key = "";
        state = state_e::UNKNOWN;
      }
    } else if ( is_double( value ) ) {
      if (is_array) {
        serialize.append( ElementDouble( std::to_string( array_key ), value ) );
        value = "";
        ++array_key;
        state = state_e::VAL_UNKNOWN;
      } else {
        serialize.append( ElementDouble( key, value ) );
        value = "";
        key = "";
        state = state_e::UNKNOWN;
      }
    } else if ( value == "null" ) {
      if (is_array) {
        serialize.append( ElementNull( std::to_string( array_key ) ) );
        value = "";
        ++array_key;
        state = state_e::VAL_UNKNOWN;
      } else {
        serialize.append( ElementNull( key ) );
        value = "";
        key = "";
        state = state_e::UNKNOWN;
      }
    } else if ( value == "true" || value == "false" ) {
      bool bool_value = ( value == "true" ) ? true : false;
      if (is_array) {
        serialize.append( ElementBool( std::to_string( array_key ), bool_value ) );
        value = "";
        ++array_key;
        state = state_e::VAL_UNKNOWN;
      } else {
        serialize.append( ElementBool( key , bool_value ) );
        value = "";
        key = "";
        state = state_e::UNKNOWN;
      }
    }
  }

  Serialize parse( const std::string& input, error_e& error, state_e start_state,
                   bool is_array ) {
    Serialize serialize;
    std::string value;
    std::string key;
    auto state = start_state;
    int array_key = 0;
    int depth = 0;

    for ( const auto& i : input ) {
      switch ( i ) {
      case '{':
        switch ( state ) {
          case state_e::UNKNOWN:

          break;
          case state_e::KEY:
            key += i;
          break;
          case state_e::SPLIT:

          break;
          case state_e::VAL_UNKNOWN:
            state = state_e::VAL_OBJECT;
            ++depth;
            value += i;
          break;
          case state_e::VAL_STRING:

          break;
          case state_e::VAL_ARRAY:
            value += i;
          break;
          case state_e::VAL_OBJECT:
            ++depth;
            value += i;
          break;
        }
      break;
      case '}':
        switch ( state ) {
          case state_e::UNKNOWN:

          break;
          case state_e::KEY:
            key += i;
          break;
          case state_e::SPLIT:

          break;
          case state_e::VAL_UNKNOWN:

          break;
          case state_e::VAL_STRING:

          break;
          case state_e::VAL_ARRAY:
            value += i;
          break;
          case state_e::VAL_OBJECT:
            --depth;
            value += i;

            if ( depth == 0 ) {
              auto reduced = value.substr( 1, value.size() - 2 );
              auto sub = parse( reduced, error, state_e::UNKNOWN, false );
              auto t_key = ( is_array ) ? std::to_string( array_key ) : key;

              if (!serialize.append( t_key, sub, error ) ) return {};

              value = "";

              if ( is_array ) {
                array_key++;
                state = state_e::VAL_UNKNOWN;
              } else {
                key = "";
                state = state_e::UNKNOWN;
              }
            }
          break;
        }
      break;
      case '[':
        switch ( state ) {
          case state_e::UNKNOWN:

          break;
          case state_e::KEY:

          break;
          case state_e::SPLIT:

          break;
          case state_e::VAL_UNKNOWN:
            state = state_e::VAL_ARRAY;
            ++depth;
            value += i;
          break;
          case state_e::VAL_STRING:

          break;
          case state_e::VAL_ARRAY:
            ++depth;
            value += i;
          break;
          case state_e::VAL_OBJECT:
            value += i;
          break;
        }
      break;
      case ']':
        switch ( state ) {
          case state_e::UNKNOWN:

          break;
          case state_e::KEY:

          break;
          case state_e::SPLIT:

          break;
          case state_e::VAL_UNKNOWN:

          break;
          case state_e::VAL_STRING:

          break;
          case state_e::VAL_ARRAY:
            --depth;
            value += i;

            if ( depth == 0 ) {
              auto reduced = value.substr( 1, value.size() - 2 );
              auto sub = parse( reduced, error, state_e::VAL_UNKNOWN, true );
              auto t_key = ( is_array ) ? std::to_string( array_key ) : key;

              if ( !serialize.appendArray( t_key, sub, error ) ) return {};

              value = "";

              if ( is_array ) {
                ++array_key;
                state = state_e::VAL_UNKNOWN;
              } else {
                key = "";
                state = state_e::UNKNOWN;
              }
            }
          break;
          case state_e::VAL_OBJECT:
            value += i;
          break;
        }
      break;
      case '\"':
        switch ( state ) {
        case state_e::UNKNOWN:
          state = state_e::KEY;
          break;
        case state_e::KEY:
          state = state_e::SPLIT;
          break;
        case state_e::SPLIT:

          break;
        case state_e::VAL_UNKNOWN:
          state = state_e::VAL_STRING;
          break;
        case state_e::VAL_STRING:
          if ( value.size() > 0 ) {
            if ( value[value.size() - 1] == '\\' ) {
              value += i;
            } else {
              if ( is_array ) {
                serialize.append( ElementString( std::to_string( array_key ), value ) );
                ++array_key;
                value = "";
                state = state_e::VAL_UNKNOWN;
              } else {
                serialize.append( ElementString( key, value ) );
                key = "";
                value = "";
                state = state_e::UNKNOWN;
              }
            }
          }
          break;
        case state_e::VAL_ARRAY:
          value += i;
        break;
        case state_e::VAL_OBJECT:
          value += i;
        break;
        }
        break;
      case ':':
        switch ( state ) {
        case state_e::UNKNOWN:

          break;
        case state_e::KEY:
          key += i;
          break;
        case state_e::SPLIT:
          state = state_e::VAL_UNKNOWN;
          break;
        case state_e::VAL_UNKNOWN:

          break;
        case state_e::VAL_STRING:
          value += i;
          break;
        case state_e::VAL_ARRAY:
          value += i;
          break;
        case state_e::VAL_OBJECT:
          value += i;
          break;
        }
        break;
      case ',':
        switch ( state ) {
        case state_e::UNKNOWN:

          break;
        case state_e::KEY:
          value += i;
          break;
        case state_e::SPLIT:

          break;
        case state_e::VAL_UNKNOWN:
          store_value( is_array, serialize, array_key, key, value, state );
          break;
        case state_e::VAL_STRING:
          value += i;
          break;
        case state_e::VAL_ARRAY:
          value += i;
          break;
        case state_e::VAL_OBJECT:
          value += i;
          break;
        }
        break;
      case '\n':
      case '\r' :
      case ' ':
        switch ( state ) {
          case state_e::UNKNOWN:

          break;
          case state_e::KEY:
            key += i;
            break;
          case state_e::SPLIT:

          break;
          case state_e::VAL_UNKNOWN:

          break;
          case state_e::VAL_STRING:
            value += i;
          break;
          case state_e::VAL_ARRAY:

          break;
          case state_e::VAL_OBJECT:
            value += i;
          break;
        }
      break;
      default:
        switch ( state ) {
          case state_e::UNKNOWN:

          break;
          case state_e::KEY:
            key += i;
          break;
          case state_e::SPLIT:

          break;
          case state_e::VAL_UNKNOWN:
            value += i;
          break;
          case state_e::VAL_STRING:
            value += i;
          break;
          case state_e::VAL_ARRAY:
            value += i;
          break;
          case state_e::VAL_OBJECT:
            value += i;
          break;
        }
      break;
      }
    }

    if ( value != "" ) {
      store_value( is_array, serialize, array_key, key, value, state );
    }

    return serialize;
  }
}

  Serialize convert( const std::string& input, error_e& error ) {
    auto reduced = input.substr( 1, input.size() - 2 );
    return parse( reduced, error, state_e::UNKNOWN, false );
  }

  Serialize convert( const std::vector<uint8_t>& input, error_e& error ) {
    std::string conv( input.begin(), input.end() );
    auto reduced = conv.substr( 1, conv.size() - 2 );
    return parse( reduced, error, state_e::UNKNOWN, false );
  }
}
}
}