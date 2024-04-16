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
  cas = 0;
}

std::pair<std::chrono::time_point<std::chrono::system_clock>, uint64_t> LocalRateLimiterImpl::getTimeout(){
  // std::lock_guard<std::mutex> lock(mutex_);
  return std::make_pair(last_timeout, cas);
}

bool LocalRateLimiterImpl::setTimeout(std::chrono::time_point<std::chrono::system_clock> timeout, uint64_t cas_){
  // std::lock_guard<std::mutex> lock(mutex_);
  ENVOY_LOG(warn, "CAS Critical Section");
  if (cas_ == cas){
    last_timeout = timeout;
    cas += 1;
    return true;
  }
  return false;
}

LocalRateLimiterImpl::~LocalRateLimiterImpl() {
}

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
