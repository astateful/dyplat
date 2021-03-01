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

#include <string>
#include <memory>

namespace astateful {
namespace plugin {
  //!
  struct Node;

  //! Given a path to a directory, walk through all entries in the directory,
  //! returning each entry wrapped in a node structure.
  struct Iterate final {
    //! Construct an iterate using the absolute path to the directory. It is
    //! assumed that the directory is valid.
    //!
    //! @param path The absolute path to the directory to iterate over.
    //!
    explicit Iterate( const std::wstring& path );
    Iterate() = delete;

    //! A custom copy constructor is required to handle the node member.
    //!
    //! @rhs The class whose contents we are constructing from.
    //!
    Iterate( const Iterate& rhs );

    //! A custom assignment operator is required to handle the node member.
    //!
    //! @rhs The class whose contents we are constructing from.
    //!
    Iterate& operator=( const Iterate& rhs );

    //! pre increment operator
    //!
    Iterate& operator++();

    //! post increment operator.
    //!
    Iterate operator++( int );

    //! boolean operator
    //!
    operator bool() const;

    //! Return a reference to the current node in the iteration.
    //!
    const Node& operator*() const { return *m_node; }

    //! Return a pointer to the current node in the iteration.
    //!
    const Node * operator->() const { return m_node.get(); }
  private:
    //! The handle acts as a global link list since we can use the
    //! current file handle to find location of the next file handle.
    void * m_handle;

    //! The absolute directory path to interate over.
    std::wstring m_path;

    //! The current node that is updated over the course of iteration.
    std::unique_ptr<Node> m_node;
  };
}
}
