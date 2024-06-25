#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/common/logger.h"
#include "src/meta_protocol_proxy/codec/codec.h"
#include "protocol.h"  // Include the Memcached protocol header

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

enum class MongoDBDecodeStatus {
  DecodeHeader,
  DecodeBody,
  DecodeDone,
  WaitForData,
};

class MongoCodec : public MetaProtocolProxy::Codec,
                   public Logger::Loggable<Logger::Id::mongo> {
public:
  MongoCodec() {}
  ~MongoCodec() override = default;

  // For protocol decoding, the buffer needs to be parsed and populated with MongoDB messages.
  MetaProtocolProxy::DecodeStatus decode(Buffer::Instance& buffer,
                                         MetaProtocolProxy::Metadata& metadata) override;

  // Protocol encoding, the request or response packet can be modified according to Mutation.
  void encode(const MetaProtocolProxy::Metadata& metadata,
              const MetaProtocolProxy::Mutation& mutation, Buffer::Instance& buffer) override;

  // Handling errors and encoding error messages to the client.
  void onError(const MetaProtocolProxy::Metadata& metadata,
               const MetaProtocolProxy::Error& error, Buffer::Instance& buffer) override;

private:
  // Implement decoding and encoding methods as needed for MongoDB messages.

  MongoDBDecodeStatus handleState(Buffer::Instance& buffer);
  MongoDBDecodeStatus decodeHeader(Buffer::Instance& buffer);
  MongoDBDecodeStatus decodeBody(Buffer::Instance& buffer);
  void toMetadata(MetaProtocolProxy::Metadata& metadata);

  MongoDBDecodeStatus decode_status_{MongoDBDecodeStatus::DecodeHeader};
  MetaProtocolProxy::MongoDB::MongoDBHeader mongo_header_;
  MetaProtocolProxy::MessageType message_type_;
  std::unique_ptr<Buffer::OwnedImpl> origin_msg_;

};

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
