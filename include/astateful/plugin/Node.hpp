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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <cstdint>

#include "MIME.hpp"

namespace astateful {
namespace plugin {
  //! A node is an item in the file system which can be either a directory or
  //! an actual file. We map each non-directory node to a MIME type to determine
  //! if the node is suitable to be displayed on the internet. We read all node
  //! data through the operating system based extensions.
  struct Node {
    //! A static list which is populated at compile time. It lists all
    //! MIME types that are supported by the server.
    static mime_t m_mime;

    //! We require that every node class be represented by an actual node
    //! on the file system. Thus declaring an empty node makes no sense.
    //!
    Node() = delete;

    //! Construct a node from the inputted location of a valid file. If the
    //! file is not valid then the node will essentially be unreliable.
    //!
    explicit Node( const std::wstring& );

    //! Return if the node is a directory or not. This property is
    //! determined directly from the node attributes provided by the
    //! operating system.
    //!
    bool directory() const { return m_directory; }

    //! Return the size of this node. This property is determined directly
    //! from the node attributes provided by the operating system. The
    //! size is computed each time since its not often that this method
    //! would be needed to be called more than once per instance.
    //!
    int64_t size() const;

    //! Return the name of this node as a wide string.
    //!
    const std::wstring& name() const { return m_name; }

    //! Decide if this node should be displayed when used within an
    //! iterator. Typically this means to show the node if it is a
    //! directory, and if not, if it has an extension that maps to a
    //! MIME type.
    //!
    bool displayable() const;

    //! Return the last time that this node was created.
    //!
    const FILETIME& created() const;

    //! Return the last time that this node was accessed.
    //!
    const FILETIME& accessed() const;

    //! Return the last time that this node was altered.
    //!
    const FILETIME& altered() const;

    //! Return the W3C content type that corresponds to this node.
    //!
    const std::string contentType() const;

    //! Return the extension for this node. This operation only makes
    //! sense if the node is a file.
    //!
    const std::wstring& extension() const { return m_extension; }

    //! Return the full original path to the node.
    //!
    const std::wstring path() const { return m_path; }

    //! less than operator
    //!
    bool operator<( const Node& ) const;
  private:
    //! Whether this node is a directory or not. We often require this
    //! information so we store it separately from the attribute data.
    bool m_directory;

    //! The name of this node as displayed in the filesystem.
    std::wstring m_name;

    //! The Windows attribute data associated with this node.
    WIN32_FILE_ATTRIBUTE_DATA m_attribute_data;

    //! The extension for this node. Only makes sense if node is a file.
    std::wstring m_extension;

    //! The full original path to the node.
    std::wstring m_path;
  };
}
}
