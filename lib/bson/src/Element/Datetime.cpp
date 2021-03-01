#include "astateful/bson/Element/Datetime.hpp"

#include <codecvt>
#include <cassert>
#include <ctime>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace astateful {
namespace bson {
  //! Due to Windows weirdness, only filetimes can be converted to
  //! a large integer structure from which times can be added and
  //! subtracted.
  //!
  inline void to_large_int( const FILETIME& in, ULARGE_INTEGER& out ) {
    out.LowPart = in.dwLowDateTime;
    out.HighPart = in.dwHighDateTime;
  }

  ElementDatetime::ElementDatetime( const std::string& key, int64_t data ) :
    m_data( data ),
    Element( key ) {}

  ElementDatetime::ElementDatetime( const std::string& key ) :
    m_data( 1000 * time( nullptr ) ),
    Element( key ) {}

  ElementDatetime::ElementDatetime( const std::string& key,
                                    const FILETIME& data ) :
    Element( key ) {
    // Doing it this way rather than using a fixed offset ensures
    // that leap boundary conditions will be properly dealt with. 
    // This should also only be created once...somewhere.
    SYSTEMTIME linux_systemtime;
    linux_systemtime.wDay = 1;
    linux_systemtime.wDayOfWeek = 4;
    linux_systemtime.wHour = 0;
    linux_systemtime.wMilliseconds = 0;
    linux_systemtime.wMinute = 0;
    linux_systemtime.wMonth = 1;
    linux_systemtime.wSecond = 0;
    linux_systemtime.wYear = 1970;

    FILETIME linux_filetime;
    SystemTimeToFileTime( &linux_systemtime, &linux_filetime );

    ULARGE_INTEGER linux_time;
    to_large_int( linux_filetime, linux_time );

    ULARGE_INTEGER windows_time;
    to_large_int( data, windows_time );

    windows_time.QuadPart -= linux_time.QuadPart;

    // Convert nanoseconds to seconds, then to milliseconds.
    m_data = windows_time.QuadPart / 10000000 * 1000;
  }

  ElementDatetime::ElementDatetime( ElementDatetime&& rhs ) :
    Element( std::move( rhs ) ),
    m_data( std::move( rhs.m_data ) ) {}

  ElementDatetime::operator std::vector<uint8_t>() const {
    std::vector<uint8_t> output( sizeof( int64_t ));
    auto temporary = m_data;

    auto data = reinterpret_cast<uint8_t *>( &temporary );
    for ( auto i = 0; i < output.size(); ++i ) {
      output[i] = *data;
      data++;
    }

    return output;
  }

  ElementDatetime::operator std::wstring() const {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes( *this );
  }

  ElementDatetime::operator std::string() const {
    time_t now = m_data / 1000;
    tm out;

    assert( !_gmtime64_s( &out, &now ) );

    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime( buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", &out );

    return buf;
  }

  std::wstring ElementDatetime::json( bool format,
                                      const std::wstring& tab,
                                      const std::wstring& indent,
                                      const int depth ) const {
    auto value = escapeJSON( *this );
    return ( format ) ? L"Datetime(\"" + value + L"\")" : L"\"" + value + L"\"";
  }

  std::vector<uint8_t> ElementDatetime::bson( error_e& ) const {
    return *this;
  }
}
}
