// op_msg.cc
#include "op_msg.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

OP_MSG::OP_MSG(const MongoDBHeader& header, int32_t flagBits, const std::vector<std::string>& sections,
               const std::string& checksum)
    : header_(header), flagBits_(flagBits), sections_(sections), checksum_(checksum) {}

bool OP_MSG::encode(Buffer::Instance& buffer) {
  // Calculate message length including header, flags, and sections.
  int32_t messageLength = MongoDBHeader::HEADER_SIZE + 4 + // flags
                         4; // section length

  for (const std::string& section : sections_) {
    messageLength += section.size() + 1; // section content + null-terminated C-string
  }

  if (!header_.encode(buffer)) {
    return false;
  }

  buffer.writeLEInt(flagBits_);
  buffer.writeLEInt(static_cast<int32_t>(sections_.size())); // Number of sections

  for (const std::string& section : sections_) {
    buffer.writeLEInt(section.size() + 1); // Section length including null-terminated C-string
    buffer.add(section);
    buffer.add("\0"); // Null-terminated C-string
  }

  return true;
}

bool OP_MSG::decode(Buffer::Instance& buffer) {
  if (!header_.decode(buffer)) {
    return false;
  }

  if (buffer.length() < 12) {
    return false; // Insufficient data for OP_MSG message
  }

  buffer.drain(4); // Skip reserved bytes
  flagBits_ = buffer.drainLEInt();

  int32_t numSections = buffer.drainLEInt();
  sections_.clear();

  for (int32_t i = 0; i < numSections; i++) {
    int32_t sectionLength = buffer.drainLEInt();
    if (sectionLength < 0 || buffer.length() < sectionLength) {
      return false; // Invalid section length or insufficient data
    }
    sections_.emplace_back(buffer.toString(sectionLength - 1)); // Read section content (without null-terminated)
    buffer.drain(1); // Skip null-terminated byte
  }

  // Check for checksum
  if (buffer.length() >= 4) {
    checksum_ = buffer.drainString(4);
  }

  return true;
}

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
