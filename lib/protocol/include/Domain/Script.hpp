//! The MIT License (MIT)
//!
//! Copyright (c) 2014 Thomas Kovacs
//!
//! Permission is hereby granted, free of charge, to any person obtaining a
//! copy of this software and associated documentation files (the "Software"),
//! to deal in the Software without restriction, including without limitation
//! the rights to use, copy, modify, merge, publish, distribute, sublicense,
//! and / or sell copies of the Software, and to permit persons to whom the
//! Software is furnished to do so, subject to the following conditions :
//!
//! The above copyright notice and this permission notice shall be included in
//! all copies or substantial portions of the Software.
//!
//! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//! FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//! DEALINGS IN THE SOFTWARE.
#pragma once

#include "../Domain.hpp"
#include "../State/Script.hpp"

#include <codecvt>

namespace astateful {
namespace protocol {
  //! Script domain acts as a frontend for running an interpreted script.
  template<typename U, typename V = std::string> struct DomainScript :
    protocol::Domain<U, V> {
    //!
    //!
    DomainScript( algorithm::Context<U, V>& algorithm,
                  const std::wstring& script_path,
                  const std::wstring& memoize_path ) :
      Domain<U, V>( algorithm ),
      m_script_path( script_path ),
      m_memoize_path( memoize_path) {}
  protected:
    //!
    const std::wstring& m_script_path;

    //!
    const std::wstring& m_memoize_path;

    //!
    //!
    bool h1( const token::h1::Context& h1 ) override {
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
      plugin::Iterate iterate( m_script_path );
      std::string index;

      m_algorithm.exec = "script";

      std::string script_path_bytes = convert.to_bytes( m_script_path );
      std::string cache_path_bytes = convert.to_bytes( m_memoize_path + L"\\llvm" );

      while ( iterate ) {
        if ( iterate->extension() == L"as" ) {
          index = convert.to_bytes( iterate->name() );

          m_algorithm.state[index] = std::make_unique<StateScript<U, V>>(
            script_path_bytes,
            cache_path_bytes,
            index );
        }

        ++iterate;
      }

      return true;
    }
  };
}
}