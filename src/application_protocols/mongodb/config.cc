#include "envoy/registry/registry.h"

#include "src/meta_protocol_proxy/codec/factory.h"
#include "src/application_protocols/mongodb/config.h"
#include "src/application_protocols/mongodb/mongo_codec.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace MongoDB {

MetaProtocolProxy::CodecPtr MongoDBCodecConfig::createCodec(const Protobuf::Message&) {
  return std::make_unique<MongoDB::MongoCodec>();
};

/**
 * Static registration for the MongoDB codec. @see RegisterFactory.
 */
REGISTER_FACTORY(MongoDBCodecConfig, MetaProtocolProxy::NamedCodecConfigFactory);

} // namespace MongoDB
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy