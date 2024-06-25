#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

struct MsgHeader {
    int32_t messageLength;
    int32_t requestID;
    int32_t responseTo;
    int32_t opCode;
};

class MongoDBHeader : public Logger::Loggable<Logger::Id::filter> {
public:
  MsgHeader header_;

  bool decode(Buffer::Instance& buffer);
  bool encode(Buffer::Instance& buffer);
  int32_t getMessageLength() const { return header_.messageLength; }
};

// Define additional MongoDB message structures and constants here

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy