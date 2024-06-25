#include "protocol.h"

namespace MongoDB {

bool MongoDBHeader::decode(Buffer::Instance& buffer) {
  if (buffer.length() < sizeof(MsgHeader)) {
    ENVOY_LOG(error, "MongoDB Header decode buffer.length:{} < {}.", buffer.length(), HEADER_SIZE);
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

  ASSERT(pos == HEADER_SIZE);

  return true;
}

bool MongoDBHeader::encode(Buffer::Instance& buffer) {
  buffer.writeLEInt(header_.messageLength_);
  buffer.writeLEInt(header_.requestID_);
  buffer.writeLEInt(header_.responseTo_);
  buffer.writeLEInt(header_.opCode_);
  return true;
}

// Implement more methods for handling MongoDB messages

} // namespace MongoDB
