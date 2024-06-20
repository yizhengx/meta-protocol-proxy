#pragma once

#include "envoy/buffer/buffer.h"
#include "envoy/common/optref.h"
#include "envoy/common/pure.h"

#include "source/common/buffer/buffer_impl.h"
#include "source/common/common/logger.h"

#include "src/meta_protocol_proxy/codec/codec.h"
#include "src/application_protocols/brpc/brpc_meta.pb.h"
#include "src/application_protocols/brpc/protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Brpc {

enum class BrpcDecodeStatus {
  DecodeHeader,
  DecodePayload,
  DecodeDone,
  WaitForData,
};

/**
 * Codec for Brpc protocol.
 */
class BrpcCodec : public MetaProtocolProxy::Codec,
                  public Logger::Loggable<Logger::Id::misc> {
public:
  BrpcCodec() {
    ENVOY_LOG(warn, "Brpc codec created");
  };
  ~BrpcCodec() override = default;

  MetaProtocolProxy::DecodeStatus decode(Buffer::Instance& buffer,
                                         MetaProtocolProxy::Metadata& metadata) override;
  void encode(const MetaProtocolProxy::Metadata& metadata,
              const MetaProtocolProxy::Mutation& mutation, Buffer::Instance& buffer) override;
  void onError(const MetaProtocolProxy::Metadata& metadata, const MetaProtocolProxy::Error& error,
               Buffer::Instance& buffer) override;

protected:
  BrpcDecodeStatus handleState(Buffer::Instance& buffer);
  BrpcDecodeStatus decodeHeader(Buffer::Instance& buffer);
  BrpcDecodeStatus decodeBody(Buffer::Instance& buffer);
  void toMetadata(MetaProtocolProxy::Metadata& metadata);

private:
  BrpcDecodeStatus decode_status{BrpcDecodeStatus::DecodeHeader};
  MetaProtocolProxy::MessageType messageType_;
  BrpcHeader brpc_header_;
  aeraki::meta_protocol::brpc::RpcMeta meta_;
  std::unique_ptr<Buffer::OwnedImpl> origin_msg_;
};

} // namespace Brpc
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
