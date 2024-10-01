#pragma once

#include "src/meta_protocol_proxy/codec/factory.h"
#include "src/application_protocols/redis/redis_codec.pb.h"
#include "src/application_protocols/redis/redis_codec.pb.validate.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Redis {

class RedisCodecConfig
    : public MetaProtocolProxy::CodecFactoryBase<aeraki::meta_protocol::codec::RedisCodec> {
public:
  RedisCodecConfig() : CodecFactoryBase("aeraki.meta_protocol.codec.redis") {}
  MetaProtocolProxy::CodecPtr createCodec(const Protobuf::Message& config) override;
};

} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy