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

#include <cassert>

namespace astateful {
namespace bson {
  Iterate::Iterate( const std::vector<uint8_t>& data ) :
    m_data( data ),
    m_position( 4 ),
    m_buffer( 0 ),
    m_state( state_e::TYPE ),
    m_element( nullptr )
  {
    bool sentinel = false;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          assert( false );
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          PushElement( m_data[i] );

          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel ) { break; }
    }
  }

  Iterate::Iterate( const Serialize& input, error_e& error ) :
    m_position( 4 ),
    m_buffer( 0 ),
    m_state( state_e::TYPE ),
    m_element( nullptr ) {
    assert( input.bson( m_data, error ) );

    bool sentinel = false;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          assert( false );
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          this->PushElement ( m_data[i] );

          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel ) { break; }
    }
  }

  Iterate& Iterate::operator++()
  {
    m_buffer.clear();
    m_state = state_e::TYPE;
    bool sentinel = false;
    int delta = 0;
    int size = 0;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return * this;
          break;
        case state_e::NAME :
          m_buffer.clear();
          delta++;
          m_state = state_e::END;
          break;
        case state_e::NAME_SIZE :
          m_buffer.clear();
          delta++;
          m_state = state_e::SIZE;
          break;
        case state_e::REGEX :
          m_buffer.clear();
          delta++;
          m_state = state_e::PATTERN;
          break;
        case state_e::PATTERN :
          delta++;
          m_state = state_e::OPTION;
          break;
        case state_e::OPTION :
          delta++;
          m_state = state_e::END;
          break;
        case state_e::SIZE :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( size );
            m_buffer.clear();
            m_state = state_e::END;

            delta += size;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          switch ( ( element_e )m_data[i] )
          {
          case element_e::EMPTY :
            delta = 0 + 1;
            m_state = state_e::NAME;
            break;
          case element_e::BOOL :
            delta = 1 + 1;
            m_state = state_e::NAME;
            break;
          case element_e::INT :
            delta = 4 + 1;
            m_state = state_e::NAME;
            break;
          case element_e::LONG :
          case element_e::DOUBLE :
          case element_e::TIMESTAMP :
          case element_e::DATETIME :
            delta = 8 + 1;
            m_state = state_e::NAME;
            break;
          case element_e::OID :
            delta = 12 + 1;
            m_state = state_e::NAME;
            break;
          case element_e::STRING :
          case element_e::CODE :
            delta = 4 + 1;
            m_state = state_e::NAME_SIZE;
            break;
          case element_e::BINARY :
            delta = 5 + 1;
            m_state = state_e::NAME_SIZE;
            break;
          case element_e::OBJECT :
          case element_e::ARRAY :
          case element_e::CODEWSCOPE :
            delta = 1;
            m_state = state_e::NAME_SIZE;
            break;
          case element_e::REGEX :
            delta = 1;
            m_state = state_e::REGEX;
            break;
          case element_e::EOO :
          default:
            return * this;
            break;
          }
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          delta++;
          break;
        case state_e::NAME_SIZE :
          m_buffer.push_back( m_data[i] );
          delta++;
          break;
        case state_e::REGEX :
          m_buffer.push_back( m_data[i] );
          delta++;
          break;
        case state_e::PATTERN :
          delta++;
          break;
        case state_e::OPTION :
          delta++;
          break;
        case state_e::SIZE :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( size );
            m_buffer.clear();
            m_state = state_e::END;

            delta += size;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        m_position += delta;

        PushElement( m_data[m_position] );

        break;
      }
    }

    return *this;
  }

  Iterate::operator bool() const {
    if ( static_cast<element_e>( m_data[m_position] ) == element_e::EOO )
      return false;

    return true;
  }

  Element& Iterate::operator*() {
    return *m_element;
  }

  Element * Iterate::operator->() {
    return m_element.get();
  }

  element_e Iterate::enumId() const {
    return static_cast<element_e>( *m_element );
  }

  std::unique_ptr<Element>& Iterate::current()
  {
    return m_element;
  }

  void Iterate::PushElement( uint8_t iElement )
  {
    m_state = state_e::TYPE;
    m_buffer.clear();

    switch ( static_cast <element_e> ( iElement ) )
    {
    case element_e::ARRAY :
      m_element.reset ( this->GenerateElementArray () );
      break;
    case element_e::BINARY :
      m_element.reset ( this->GenerateElementBinary () );
      break;
    case element_e::BOOL :
      m_element.reset ( this->GenerateElementBool () );
      break;
    case element_e::CODE :
      m_element.reset ( this->GenerateElementCode () );
      break;
    case element_e::CODEWSCOPE :
      m_element.reset ( this->GenerateElementCodescope () );
      break;
    case element_e::DATETIME :
      m_element.reset ( this->GenerateElementDatetime () );
      break;
    case element_e::DOUBLE :
      m_element.reset ( this->GenerateElementDouble () );
      break;
    case element_e::INT :
      m_element.reset ( this->GenerateElementInt () );
      break;
    case element_e::LONG :
      m_element.reset ( this->GenerateElementLong () );
      break;
    case element_e::EMPTY :
      m_element.reset ( this->GenerateElementNull () );
      break;
    case element_e::OBJECT :
      m_element.reset ( this->GenerateElementObject () );
      break;
    case element_e::OID :
      m_element.reset ( this->GenerateElementObjectId () );
      break;
    case element_e::REGEX:
      m_element.reset ( this->GenerateElementRegex () );
      break;
    case element_e::STRING :
      m_element.reset ( this->GenerateElementString () );
      break;
    case element_e::TIMESTAMP :
      m_element.reset ( this->GenerateElementTimestamp () );
      break;
    case element_e::EOO :
      break;
    };
  }

  Element * Iterate::GenerateElementArray()
  {
    std::string key( "" );
    bool sentinel = false;
    size_t delta = 0;
    int total = 0;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE:
          return nullptr;
          break;
        case state_e::NAME:
          delta = i - m_position;

          ToString( key );
          m_buffer.clear();
          m_state = state_e::SIZE;
          break;
        case state_e::SIZE:
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( total );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END:
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE:
          m_state = state_e::NAME;
          break;
        case state_e::NAME:
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::SIZE:
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( total );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END:
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        m_buffer.assign( m_data.begin() + m_position + delta + 1,
                          m_data.begin() + m_position + delta + 1 + total );

        return new ElementArray( key, m_buffer );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementBinary()
  {
    std::string key ( "" );
    bool sentinel = false;
    int total;
    int sub = 0;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( total );
            m_buffer.clear();
            m_state = state_e::SUBTYPE;
          }
          break;
        case state_e::SUBTYPE :
          m_buffer.push_back( m_data[i] );
          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( total );
            m_buffer.clear();
            m_state = state_e::SUBTYPE;
          }
          break;
        case state_e::SUBTYPE :
          m_buffer.push_back( m_data[i] );
          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        sub = m_buffer [0];

        m_buffer.assign (
          m_data.begin () + i,
          m_data.begin () + i + total
        );

        switch ( ( ElementBinary::sub_e )abs( sub ) )
        {
        case ElementBinary::sub_e::GENERIC :
          return new ElementBinaryGeneric( key, m_buffer );
          break;
        case ElementBinary::sub_e::FUNCTION :
          return new ElementBinaryFunction( key, m_buffer );
          break;
        case ElementBinary::sub_e::UUID :
          return new ElementBinaryUUID( key, m_buffer );
          break;
        case ElementBinary::sub_e::MD5 :
          return new ElementBinaryMD5( key, m_buffer );
          break;
        case ElementBinary::sub_e::UDEF :
          return new ElementBinaryUDEF( key, m_buffer );
          break;
        default :
          return nullptr;
          break;
        }

        break;
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementBool ()
  {
    std::string key ( "" );
    bool sentinel = false;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );
          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );
          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        switch ( m_buffer [0] )
        {
        case '\x0' :
          return new ElementBool( key, false );
          break;
        case '\x1' :
          return new ElementBool( key, true );
          break;
        }

        break;
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementCode ()
  {
    std::string key ( "" );
    bool sentinel = false;
    int size;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( size );
            m_buffer.clear();
            m_state = state_e::STRING;
          }
          break;
        case state_e::STRING :
          if ( m_buffer.size() + 1 == size )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( size );
            m_buffer.clear();
            m_state = state_e::STRING;
          }
          break;
        case state_e::STRING :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() + 1 == size )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementCode( key, m_buffer );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementCodescope ()
  {
    std::vector <uint8_t> save ( 0 );
    std::string key ( "" );
    bool sentinel = false;
    size_t delta = 0;
    int total = 0;
    int code_size;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          delta = i - m_position;

          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::SIZE;
          break;
        case state_e::SIZE :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( total );
            m_buffer.clear();
            m_state = state_e::CODE_SIZE;
          }
          break;
        case state_e::CODE_SIZE :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( code_size );
            m_buffer.clear();
            m_state = state_e::ELEMENT;
          }
          break;
        case state_e::ELEMENT :
          save.push_back ( m_data[i] );

          if ( save.size () + 1 == code_size )
          {
            save.pop_back();

            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::SIZE :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( total );
            m_buffer.clear();
            m_state = state_e::CODE_SIZE;
          }
          break;
        case state_e::CODE_SIZE :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( code_size );
            m_buffer.clear();
            m_state = state_e::ELEMENT;
          }
          break;
        case state_e::ELEMENT :
          save.push_back ( m_data[i] );

          if ( save.size () + 1 == code_size )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        m_buffer.assign (
          m_data.begin () + m_position + delta + 1 + 4 + 4 + code_size,
          m_data.begin () + m_position + delta + 1 + total
        );

        return new ElementCodescope ( key, save, m_buffer );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementDatetime()
  {
    std::string key ( "" );
    bool sentinel = false;
    int64_t value;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 8 )
          {
            this->ToLong ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 8 )
          {
            this->ToLong ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementDatetime ( key, value );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementDouble ()
  {
    std::string key ( "" );
    bool sentinel = false;
    double value;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 8 )
          {
            this->ToDouble ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 8 )
          {
            this->ToDouble ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementDouble ( key, value );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementInt ()
  {
    std::string key ( "" );
    bool sentinel = false;
    int32_t value;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementInt ( key, value );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementLong()
  {
    std::string key ( "" );
    bool sentinel = false;
    int64_t value;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 8 )
          {
            this->ToLong ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 8 )
          {
            this->ToLong ( value );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementLong ( key, value );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementNull ()
  {
    std::string key ( "" );
    bool sentinel = false;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString ( key );
          m_buffer.clear();
          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementNull( key );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementObject()
  {
    std::string key ( "" );
    bool sentinel = false;
    size_t delta = 0;
    int total = 0;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE:
          return nullptr;
          break;
        case state_e::NAME:
          delta = i - m_position;

          ToString( key );
          m_buffer.clear();
          m_state = state_e::SIZE;
          break;
        case state_e::SIZE:
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( total );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END:
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE:
          m_state = state_e::NAME;
          break;
        case state_e::NAME:
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::SIZE:
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( total );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END:
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        m_buffer.assign( m_data.begin() + m_position + delta + 1,
                          m_data.begin() + m_position + delta + 1 + total );

        return new ElementObject( key, m_buffer );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementObjectId ()
  {
    std::string key ( "" );
    bool sentinel = false;

    for ( size_t i = m_position; i < m_data.size(); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 12 )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 12 )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementObjectId( key, m_buffer );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementRegex()
  {
    std::vector<uint8_t> save( 0 );
    std::string key ( "" );
    bool sentinel = false;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_state = state_e::OPTION;
          break;
        case state_e::OPTION :
          m_state = state_e::END;
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::OPTION :
          save.push_back ( m_data[i] );
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementRegex( key, m_buffer, save );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementString()
  {
    std::string key ( "" );
    bool sentinel = false;
    int size = 0;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          this->ToString( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( size );
            m_buffer.clear();
            m_state = state_e::STRING;
          }
          break;
        case state_e::STRING :
          if ( m_buffer.size() + 1 == size )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            this->ToInt ( size );
            m_buffer.clear();
            m_state = state_e::STRING;
          }
          break;
        case state_e::STRING :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() + 1 == size )
          {
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementString( key, m_buffer );
      }
    }

    return nullptr;
  }

  Element * Iterate::GenerateElementTimestamp()
  {
    std::string key ( "" );
    bool sentinel = false;
    int32_t increment = 0;
    int32_t timestamp = 0;

    for ( size_t i = m_position; i < m_data.size (); ++i )
    {
      switch ( m_data[i] )
      {
      case '\0' :
        switch ( m_state )
        {
        case state_e::TYPE :
          return nullptr;
          break;
        case state_e::NAME :
          ToString( key );
          m_buffer.clear();
          m_state = state_e::ELEMENT;
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( increment );
            m_buffer.clear();
            m_state = state_e::TIMESTAMP;
          }
          break;
        case state_e::TIMESTAMP :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( timestamp );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      default :
        switch ( m_state )
        {
        case state_e::TYPE :
          m_state = state_e::NAME;
          break;
        case state_e::NAME :
          m_buffer.push_back( m_data[i] );
          break;
        case state_e::ELEMENT :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( increment );
            m_buffer.clear();
            m_state = state_e::TIMESTAMP;
          }
          break;
        case state_e::TIMESTAMP :
          m_buffer.push_back( m_data[i] );

          if ( m_buffer.size() == 4 )
          {
            ToInt( timestamp );
            m_buffer.clear();
            m_state = state_e::END;
          }
          break;
        case state_e::END :
          sentinel = true;
          break;
        }
        break;
      }

      if ( sentinel )
      {
        return new ElementTimestamp( key, increment, timestamp );
      }
    }

    return nullptr;
  }

  void Iterate::ToString( std::string& output ) const
  {
    output.assign( m_buffer.begin(), m_buffer.end() );
  }

  void Iterate::ToInt( int& output ) const
  {
    memcpy( &output,
            reinterpret_cast <char *> (
              &const_cast <Iterate *> ( this )->m_buffer[0]
            ), 4
          );
  }

  void Iterate::ToLong( int64_t& output ) const
  {
    memcpy( &output,
            reinterpret_cast <char *> (
              &const_cast <Iterate *> ( this )->m_buffer[0]
            ), 8
          );
  }

  void Iterate::ToDouble( double& output ) const
  {
    memcpy( &output,
            reinterpret_cast <char *> (
              &const_cast <Iterate *> ( this )->m_buffer[0]
            ), 8
          );
  };
}
}
