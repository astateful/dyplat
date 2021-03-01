#include "astateful/plugin/Query.hpp"

namespace astateful {
namespace plugin {
namespace {
  enum class state_e {
    START,
    KEY,
    VALUE
  };
}

  Query::Query( const std::string& input ) :
    m_input( input ) {}

  bool Query::operator()( query_t& result ) const {
    auto state = state_e::START;
    std::string key ( "" );
    std::string value ( "" );

    for ( const auto& i : m_input )
    {
      switch ( i )
      {
      case '=' :
        switch ( state )
        {
        case state_e::START :
          return false;
          break;
        case state_e::KEY :
          state = state_e::VALUE;
          break;
        case state_e::VALUE :
          return false;
          break;
        }
        break;
      case '&' :
        switch ( state )
        {
        case state_e::START :
          return false;
          break;
        case state_e::KEY :
          return false;
          break;
        case state_e::VALUE :
          result [key] = value;

          key = "";
          value = "";

          state = state_e::KEY;
          break;
        }
        break;
      default :
        switch ( state )
        {
        case state_e::START :
          key += i;
          state = state_e::KEY;
          break;
        case state_e::KEY :
          key += i;
          break;
        case state_e::VALUE :
          value += i;
          break;
        }
        break;
      }
    }

    switch ( state )
    {
    case state_e::START :
      return false;
      break;
    case state_e::KEY :
      return false;
      break;
    case state_e::VALUE :
      result[key] = value;
      break;
    }

    return true;
  }
}
}
