#pragma once

#include <chrono>
#include <queue>
#include <mutex>

#include "envoy/event/dispatcher.h"
#include "envoy/event/timer.h"
#include "envoy/extensions/common/ratelimit/v3/ratelimit.pb.h"
#include "envoy/ratelimit/ratelimit.h"
#include "source/common/common/logger.h"

#include "source/common/common/thread_synchronizer.h"
#include "source/common/protobuf/protobuf.h"
#include "source/common/http/header_utility.h"
#include "src/meta_protocol_proxy/codec/codec.h"
#include "src/meta_protocol_proxy/filters/filter.h"

#include "api/meta_protocol_proxy/filters/local_ratelimit/v1alpha/local_ratelimit.pb.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace LocalRateLimit {

using LocalRateLimitConfig =
aeraki::meta_protocol_proxy::filters::local_ratelimit::v1alpha::LocalRateLimit;

class LocalRateLimiterImpl: public Logger::Loggable<Logger::Id::filter> {
public:
  LocalRateLimiterImpl(
      const std::chrono::milliseconds fill_interval, const uint32_t max_tokens,
      Event::Dispatcher& dispatcher, const LocalRateLimitConfig& cfg);
  ~LocalRateLimiterImpl();

  // Custom Logic
  std::chrono::time_point<std::chrono::system_clock> getTimeout(std::chrono::time_point<std::chrono::system_clock> cur_time);

private:
  std::chrono::microseconds delay;
  std::chrono::time_point<std::chrono::system_clock> last_timeout;
  LocalRateLimitConfig config_;
  mutable std::mutex mutex_;
};

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
