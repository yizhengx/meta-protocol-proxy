#include "envoy/registry/registry.h"

#include "src/meta_protocol_proxy/codec/factory.h"
#include "src/application_protocols/memcached/config.h"
#include "src/application_protocols/memcached/memcached_codec.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

MetaProtocolProxy::CodecPtr MemcachedCodecConfig::createCodec(const Protobuf::Message&) {
  std::cout << "MemcachedCodecConfig::createCodec" << std::endl;
  return std::make_unique<Memcached::MemcachedCodec>();
};

/**
 * Static registration for the memcached codec. @see RegisterFactory.
 */
REGISTER_FACTORY(MemcachedCodecConfig, MetaProtocolProxy::NamedCodecConfigFactory);

} // namespace memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy