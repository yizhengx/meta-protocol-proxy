#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/buffer/buffer_impl.h"
#include "source/common/common/logger.h"
#include "src/meta_protocol_proxy/codec/codec.h"
#include "protocol.h"  // Include the Memcached protocol header
#include <iostream>
// include the bsoncxx library
#include <bsoncxx/builder/stream/document.hpp>

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

class MongoDBCodec : public MetaProtocolProxy::Codec,
                   public Logger::Loggable<Logger::Id::misc> {
public:
  MongoDBCodec() {
    std::cout << "MongoDBCodec constructor" << std::endl;
  }
  ~MongoDBCodec() override = default;

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

  MongoDBDecodeStatus handleState(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
  MongoDBDecodeStatus decodeHeader(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
  MongoDBDecodeStatus decodeBody(Buffer::Instance& buffer, MetaProtocolProxy::Metadata& metadata);
  void toMetadata(MetaProtocolProxy::Metadata& metadata);
  std::string buffer_to_string(Buffer::Instance& buffer, size_t length);

  MongoDBDecodeStatus decode_status_{MongoDBDecodeStatus::DecodeHeader};
  MetaProtocolProxy::MongoDB::MongoDBHeader mongo_header_;
  MetaProtocolProxy::MessageType message_type_;
  std::unique_ptr<Buffer::OwnedImpl> origin_msg_;

  bool seen_is_master = false;

};

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
