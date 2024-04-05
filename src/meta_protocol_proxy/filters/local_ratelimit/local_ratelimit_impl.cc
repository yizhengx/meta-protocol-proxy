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
    : fill_timer_(dispatcher.createTimer([this] { onFillTimer(); })),
      time_source_(dispatcher.timeSource()), timer_duration_(fill_interval),
      config_(cfg), queue(TSQueue(std::chrono::microseconds(max_tokens))){
        ENVOY_LOG(warn, "LocalRateLimiterImpl Constructor");
        
  timer_duration_ = fill_interval;
  if (timer_duration_ < std::chrono::milliseconds(50)) {
    throw EnvoyException("local rate limit token bucket fill timer must be >= 50ms");
  }
  fill_timer_->enableTimer(timer_duration_);
}

LocalRateLimiterImpl::~LocalRateLimiterImpl() {
  fill_timer_->disableTimer();
}

void LocalRateLimiterImpl::bufferRequest(DecoderFilterCallbacks* callbacks){
  ENVOY_LOG(warn, "LocalRateLimiterImpl::bufferRequest");
  queue.push(callbacks);
}

void LocalRateLimiterImpl::onFillTimer() {
  // ENVOY_LOG(warn, "LocalRateLimiterImpl::onFillTimer");
  queue.pop();
  fill_timer_->enableTimer(timer_duration_);
}


} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
