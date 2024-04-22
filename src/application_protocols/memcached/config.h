#pragma once

#include "src/meta_protocol_proxy/codec/factory.h"
#include "src/application_protocols/memcached/memcached_codec.pb.h"
#include "src/application_protocols/memcached/memcached_codec.pb.validate.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Memcached {

class MemcachedCodecConfig
    : public MetaProtocolProxy::CodecFactoryBase<aeraki::meta_protocol::codec::MemcachedCodec> {
public:
  MemcachedCodecConfig() : CodecFactoryBase("aeraki.meta_protocol.codec.memcached") {}
  MetaProtocolProxy::CodecPtr createCodec(const Protobuf::Message& config) override;
};

} // namespace Memcached
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy