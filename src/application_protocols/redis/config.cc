#include "envoy/registry/registry.h"

#include "src/meta_protocol_proxy/codec/factory.h"
#include "src/application_protocols/redis/config.h"
#include "src/application_protocols/redis/redis_codec.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Redis {

MetaProtocolProxy::CodecPtr RedisCodecConfig::createCodec(const Protobuf::Message&) {
  return std::make_unique<Redis::RedisCodec>();
};

/**
 * Static registration for the redis codec. @see RegisterFactory.
 */
REGISTER_FACTORY(RedisCodecConfig, MetaProtocolProxy::NamedCodecConfigFactory);

} // namespace redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy