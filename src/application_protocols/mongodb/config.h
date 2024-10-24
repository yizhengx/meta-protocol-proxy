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
    : public MetaProtocolProxy::CodecFactoryBase<aeraki::meta_protocol::codec::MongoDBCodec> {
public:
  MongoDBCodecConfig() : CodecFactoryBase("aeraki.meta_protocol.codec.mongodb") {
    buffer = static_cast<char*>(malloc(500 * sizeof(char)));
    buffer_size = static_cast<size_t*>(malloc(sizeof(size_t)));
    *buffer_size = 0;
  }
  MetaProtocolProxy::CodecPtr createCodec(const Protobuf::Message& config) override;
  char* buffer;
  size_t* buffer_size;
  // bool* isMaster{false};
};

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy