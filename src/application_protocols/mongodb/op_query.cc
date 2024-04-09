// op_query.cc
#include "op_query.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

OP_QUERY::OP_QUERY(const MongoDBHeader& header, int32_t flags, const std::string& fullCollectionName,
                   int32_t numberToSkip, int32_t numberToReturn, const std::string& query,
                   const std::string& returnFieldsSelector)
    : header_(header),
      flags_(flags),
      fullCollectionName_(fullCollectionName),
      numberToSkip_(numberToSkip),
      numberToReturn_(numberToReturn),
      query_(query),
      returnFieldsSelector_(returnFieldsSelector) {}

bool OP_QUERY::encode(Buffer::Instance& buffer) {
  // Calculate message length including header and query document size.
  int32_t messageLength = MongoDBHeader::HEADER_SIZE +
                         4 + // flags
                         fullCollectionName_.size() + 1 + // null-terminated C-string
                         4 + // numberToSkip
                         4 + // numberToReturn
                         query_.size() + 1; // null-terminated C-string

  if (!header_.encode(buffer)) {
    return false;
  }

  buffer.writeLEInt(flags_);
  buffer.add(fullCollectionName_);
  buffer.add("\0"); // Null-terminated C-string
  buffer.writeLEInt(numberToSkip_);
  buffer.writeLEInt(numberToReturn_);
  buffer.add(query_);
  buffer.add("\0"); // Null-terminated C-string

  return true;
}

bool OP_QUERY::decode(Buffer::Instance& buffer) {
  if (!header_.decode(buffer)) {
    return false;
  }

  if (buffer.length() < 24) {
    return false; // Insufficient data for query message
  }

  buffer.drain(4); // Skip reserved bytes
  flags_ = buffer.drainLEInt();
  fullCollectionName_ = buffer.drainStringZeroTerminated();
  numberToSkip_ = buffer.drainLEInt();
  numberToReturn_ = buffer.drainLEInt();
  query_ = buffer.drainStringZeroTerminated();
  returnFieldsSelector_ = buffer.drainStringZeroTerminated();

  return true;
}

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
