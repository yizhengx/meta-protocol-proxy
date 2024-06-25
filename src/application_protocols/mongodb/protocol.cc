#include "protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

bool MongoDBHeader::decode(Buffer::Instance& buffer) {
  if (buffer.length() < sizeof(MsgHeader)) {
    ENVOY_LOG(error, "MongoDB Header decode buffer.length:{} < {}.", buffer.length(), sizeof(MsgHeader));
    return false;
  }

  uint64_t pos = 0;
  header_.messageLength_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);
  header_.requestID_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);
  header_.responseTo_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);
  header_.opCode_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);

  ASSERT(pos == sizeof(MsgHeader));

  return true;
}

bool MongoDBHeader::encode(Buffer::Instance& buffer) {
  buffer.writeLEInt(header_.messageLength);
  buffer.writeLEInt(header_.requestID);
  buffer.writeLEInt(header_.responseTo);
  buffer.writeLEInt(header_.opCode);
  return true;
}

// Implement more methods for handling MongoDB messages

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
