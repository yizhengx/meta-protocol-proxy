#include "src/meta_protocol_proxy/filters/local_ratelimit/local_ratelimit_impl.h"
#include "src/meta_protocol_proxy/codec_impl.h"

#include "source/common/protobuf/utility.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace LocalRateLimit {

LocalRateLimiterImpl::LocalRateLimiterImpl(
    const std::chrono::milliseconds fill_interval, const uint32_t max_tokens, 
    Event::Dispatcher& dispatcher, const LocalRateLimitConfig& cfg)
    : config_(cfg)
{
  ENVOY_LOG(warn, "LocalRateLimiterImpl Constructor");
  ENVOY_LOG(warn, max_tokens);
  delay = std::chrono::microseconds(max_tokens);
  last_timeout = std::chrono::system_clock::now();
}

std::chrono::time_point<std::chrono::system_clock> LocalRateLimiterImpl::getTimeout(std::chrono::time_point<std::chrono::system_clock> cur_time){
  std::lock_guard<std::mutex> lock(mutex_);
  last_timeout = max(last_timeout, cur_time) + delay;
  return last_timeout;
}

LocalRateLimiterImpl::~LocalRateLimiterImpl() {
  fill_timer_->disableTimer();
}

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
