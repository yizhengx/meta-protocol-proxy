#include "src/meta_protocol_proxy/filters/local_ratelimit/local_ratelimit_impl.h"
#include "src/meta_protocol_proxy/codec_impl.h"

#include "source/common/protobuf/utility.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace LocalRateLimit {

LocalRateLimiterImpl::LocalRateLimiterImpl(
    const std::chrono::milliseconds, const uint32_t max_tokens, 
    Event::Dispatcher&, const LocalRateLimitConfig& cfg)
    : config_(cfg)
{
  ENVOY_LOG(warn, "LocalRateLimiterImpl Constructor");
  ENVOY_LOG(warn, max_tokens);
  delay = std::chrono::microseconds(max_tokens);
  last_timeout = std::chrono::system_clock::now();
  // cas = 0;
}

std::chrono::time_point<std::chrono::system_clock> LocalRateLimiterImpl::getTimeout(){
  // std::lock_guard<std::mutex> lock(mutex_);
  return last_timeout.load();
}

bool LocalRateLimiterImpl::setTimeout(std::chrono::time_point<std::chrono::system_clock> old_timeout, std::chrono::time_point<std::chrono::system_clock> new_timeout){
  // std::lock_guard<std::mutex> lock(mutex_);
  return last_timeout.compare_exchange_weak(old_timeout, new_timeout);
}

LocalRateLimiterImpl::~LocalRateLimiterImpl() {
}

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
