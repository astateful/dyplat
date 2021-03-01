#include "astateful/byte/h2/FrameHeader.hpp"

#include "Windows.h"

namespace astateful {
namespace byte {
namespace h2 {
    FrameHeader::FrameHeader() :
      length( 0 ),
      type( 0 ),
      flags( 0 ),
      stream_id( 0 ) {}

    FrameHeader::FrameHeader(uint32_t l, uint8_t t, uint8_t f, uint32_t sid)
      : length(l), type(t), flags(f), stream_id(sid) {}
  
    FrameHeader::FrameHeader(const uint8_t* buffer, size_t length)
      { read_from_buffer(buffer, length); }

    void FrameHeader::read_from_buffer(const uint8_t* buffer, size_t buflen) {
      if (buflen >= 9) {
        length = (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
        type = buffer[3];
        flags = buffer[4];
        stream_id = ntohl(*reinterpret_cast<const uint32_t*>(&buffer[5]));
      }
    }

    std::vector<uint8_t> FrameHeader::write_to_buffer() {
      std::vector<uint8_t> buffer;

      buffer.push_back((length >> 16) & 0xff);
      buffer.push_back((length >> 8) & 0xff);
      buffer.push_back(length & 0xff);
      buffer.push_back(type);
      buffer.push_back(flags);
      buffer.push_back((stream_id >> 24) & 0xff);
      buffer.push_back((stream_id >> 16) & 0xff);
      buffer.push_back((stream_id >> 8) & 0xff);
      buffer.push_back(stream_id & 0xff);

      return buffer;
    }

    uint32_t FrameHeader::get_length() { return length; }

    uint8_t FrameHeader::get_type() { return type; }

    uint8_t FrameHeader::get_flags() { return flags; }

    uint32_t FrameHeader::get_stream_id() { return stream_id; }
}
}
}