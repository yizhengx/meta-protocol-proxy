#include "protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Redis {

bool RedisHeader::decode(Buffer::Instance&) {
    return true;
}

bool RedisHeader::encode(Buffer::Instance&) {
    return true;
}


} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
