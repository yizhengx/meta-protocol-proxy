#pragma once

#include "envoy/buffer/buffer.h"
#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace Redis {

struct RedisHeader : public Logger::Loggable<Logger::Id::filter> {
    bool decode(Buffer::Instance& buffer);
    bool encode(Buffer::Instance& buffer);
};

} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
