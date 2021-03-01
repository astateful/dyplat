#include "net/Address.hpp"

#include "astateful/token/address/Context.hpp"
#include "astateful/token/address/Machine.hpp"

#include <iostream>
#include <codecvt>
#include <cassert>

namespace astateful {
namespace async {
namespace net {
  template<typename T> bool parse_ipv6_addr( std::wstring& hostname,
                                             std::wstring& port,
                                             T * address ) {
    hostname.resize( INET6_ADDRSTRLEN );
    const auto ipv6 = reinterpret_cast<sockaddr_in6*>( address );

    if ( InetNtopW( AF_INET6,
                    &( ipv6->sin6_addr ),
                    &hostname[0],
                    hostname.size() ) == nullptr ) return false;

    port = std::to_wstring( ipv6->sin6_port );

    return true;
  }

  template<typename T> bool parse_ipv4_addr( std::wstring& hostname,
                                             std::wstring& port,
                                             T * address ) {
    hostname.resize( INET_ADDRSTRLEN );
    auto ipv4 = reinterpret_cast<sockaddr_in*>( address );

    if ( InetNtopW( AF_INET,
                    &( ipv4->sin_addr ),
                    &hostname[0],
                    hostname.size() ) == nullptr ) return false;

    port = std::to_wstring( ipv4->sin_port );

    return true;
  }

  template<typename T> bool parse_addr( ADDRESS_FAMILY family,
                                        std::wstring& hostname,
                                        std::wstring& port,
                                        const T * addr ) {
    auto non_const_addr = const_cast<T*>( addr );

    if ( family == AF_INET6 ) {
      return parse_ipv6_addr( hostname, port, non_const_addr );
    } else if ( family == AF_INET ) {
      return parse_ipv4_addr( hostname, port, non_const_addr );
    }

    return false;
  }

  Address::Address( const SOCKADDR_STORAGE& address ) :
    m_port(),
    m_hostname() {
    assert( parse_addr( address.ss_family, m_hostname, m_port, &address ) );
  }

  Address::Address( const ADDRINFOW * address ) :
    m_port(),
    m_hostname() {
    assert( parse_addr( address->ai_family, m_hostname, m_port,
      address->ai_addr ) );
  }

  Address::Address( const token::address::Context& context ) :
    m_port( context.port ),
    m_hostname( context.hostname ) {}

  Address::Address( const Address& rhs ) :
    m_port( rhs.m_port ),
    m_hostname( rhs.m_hostname ) {}

  Address::Address( Address&& rhs ) :
    m_port( std::move( rhs.m_port ) ),
    m_hostname( std::move( rhs.m_hostname ) ) {}

  Address& Address::operator=( const Address& rhs ) {
    m_port = rhs.m_port;
    m_hostname = rhs.m_hostname;

    return *this;
  }

  Address& Address::operator=( Address&& rhs ) {
    if ( this != &rhs ) {
      m_port = std::move( rhs.m_port );
      m_hostname = std::move( rhs.m_hostname );
    }

    return *this;
  }

  Address::operator std::string() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

    std::wstring address = m_hostname;
    address += L":";
    address += m_port;

    return convert.to_bytes( address.c_str() );
  }

  bool Address::info( const ADDRINFOW& hint, PADDRINFOW& list ) const {
    if ( GetAddrInfoW( m_hostname.c_str(),
                       m_port.c_str(),
                       &hint,
                       &list ) == 0 ) return true;

    return false;
  }

  std::ostream& operator<<( std::ostream& stream, const Address& address ) {
    stream << std::string( address );
    return stream;
  }

  std::unique_ptr<Address> generate_address( const std::wstring& input ) {
    const token::address::Machine machine;
    token::address::Context context( machine.initialState() );

    for ( const auto& value : input ) {
      if ( !machine( context, context.state, value ) )
        return {};
    }

    return std::make_unique<Address>( context );
  }
}
}
}
