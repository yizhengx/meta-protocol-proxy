// op_msg.h
#pragma once

#include "protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

class OP_MSG {
public:
  OP_MSG(const MongoDBHeader& header, int32_t flagBits, const std::vector<std::string>& sections,
         const std::string& checksum = "");

  bool encode(Buffer::Instance& buffer);
  bool decode(Buffer::Instance& buffer);

private:
  const MongoDBHeader& header_;
  int32_t flagBits_;
  std::vector<std::string> sections_;
  std::string checksum_;
};

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
