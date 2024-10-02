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

    uint16_t get_key_length() const { return key_length; }
    uint32_t get_total_body_length() const { return total_body_length; }
};

} // namespace Redis
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
