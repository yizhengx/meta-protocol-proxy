// op_query.h
#pragma once

#include "protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

class OP_QUERY {
public:
  OP_QUERY(const MongoDBHeader& header, int32_t flags, const std::string& fullCollectionName,
           int32_t numberToSkip, int32_t numberToReturn, const std::string& query,
           const std::string& returnFieldsSelector = "");

  bool encode(Buffer::Instance& buffer);
  bool decode(Buffer::Instance& buffer);

private:
  const MongoDBHeader& header_;
  int32_t flags_;
  std::string fullCollectionName_;
  int32_t numberToSkip_;
  int32_t numberToReturn_;
  std::string query_;
  std::string returnFieldsSelector_;
};

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
