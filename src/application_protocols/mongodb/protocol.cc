#include "protocol.h"
#include "envoy/buffer/buffer.h"
#include "envoy/common/logger.h"

namespace MongoDB {

bool MongoDBHeader::decode(Buffer::Instance& buffer) {
  if (buffer.length() < HEADER_SIZE) {
    ENVOY_LOG(error, "MongoDB Header decode buffer.length:{} < {}.", buffer.length(), HEADER_SIZE);
    return false;
  }

  uint64_t pos = 0;
  messageLength_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);
  requestID_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);
  responseTo_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);
  opCode_ = buffer.peekLEInt<int32_t>(pos);
  pos += sizeof(int32_t);

  ASSERT(pos == HEADER_SIZE);

  return true;
}

bool MongoDBHeader::encode(Buffer::Instance& buffer) {
  buffer.writeLEInt(messageLength_);
  buffer.writeLEInt(requestID_);
  buffer.writeLEInt(responseTo_);
  buffer.writeLEInt(opCode_);
  return true;
}

// Implement more methods for handling MongoDB messages

} // namespace MongoDB
