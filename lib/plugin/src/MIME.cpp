#include "astateful/plugin/MIME.hpp"

namespace astateful {
namespace plugin {
  MIME::MIME( mime_e mime, const std::string& type ) :
    m_mime( mime ),
    m_type( type ) {}

  MIME::operator mime_e() const {
    return m_mime;
  }

  const std::string& MIME::type() const {
    return m_type;
  }
}
}
