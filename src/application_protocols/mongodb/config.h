#pragma once

#include "src/meta_protocol_proxy/codec/factory.h"
#include "src/application_protocols/mongodb/mongodb_codec.pb.h"
#include "src/application_protocols/mongodb/mongodb_codec.pb.validate.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

class MongoDBCodecConfig
    : public MetaProtocolProxy::CodecFactoryBase<aeraki::meta_protocol::codec::MongoCodec> {
public:
  MongoDBCodecConfig() : CodecFactoryBase("aeraki.meta_protocol.codec.mongodb") {}
  MetaProtocolProxy::CodecPtr createCodec(const Protobuf::Message& config) override;
};

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy