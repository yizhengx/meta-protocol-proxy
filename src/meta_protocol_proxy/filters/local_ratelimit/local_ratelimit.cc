#include "src/meta_protocol_proxy/filters/local_ratelimit/local_ratelimit.h"
#include "src/meta_protocol_proxy/app_exception.h"
#include "source/common/buffer/buffer_impl.h"
#include "envoy/extensions/filters/network/local_ratelimit/v3/local_rate_limit.pb.h"
#include "src/meta_protocol_proxy/filters/local_ratelimit/local_ratelimit_impl.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace LocalRateLimit {

FilterConfig::FilterConfig(const LocalRateLimitConfig& cfg, Stats::Scope&,
                           Event::Dispatcher& dispatcher)
    : rate_limiter_(LocalRateLimiterImpl(
          std::chrono::milliseconds(
              PROTOBUF_GET_MS_OR_DEFAULT(cfg.token_bucket(), fill_interval, 0)),
          cfg.token_bucket().max_tokens(), dispatcher,
          cfg)) {
            // ENVOY_LOG(warn, "FilterConfig Constructor");
            dispatcher = dispatcher;
          }

void LocalRateLimit::onDestroy() { cleanup(); }

void LocalRateLimit::setDecoderFilterCallbacks(DecoderFilterCallbacks& callbacks) {
  callbacks_ = &callbacks;
}

FilterStatus LocalRateLimit::onMessageDecoded(MetadataSharedPtr, MutationSharedPtr) {
  if (has_buffered) {
    // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, resumeIteration {}", *callbacks_, metadata->getRequestId());
    return FilterStatus::ContinueIteration;
  }
  // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, pauseIteration {}", *callbacks_, metadata->getRequestId());
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now()
  std::chrono::time_point<std::chrono::system_clock> timeout = 
    filter_config_->rateLimiter().getTimeout(now);
  fill_timer_ = filter_config_->dispatcher.createTimer([this] { onFillTimer(); })
  fill_timer_.enableHRTimer(std::chrono::duration_cast<std::chrono::microseconds>(timeout - now));
  has_buffered = true;
  return FilterStatus::PauseIteration;
  // return FilterStatus::ContinueIteration;
}

void LocalRateLimit::onFillTimer(){
  callbacks_->continueDecoding();
}

void LocalRateLimit::setEncoderFilterCallbacks(EncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
}

FilterStatus LocalRateLimit::onMessageEncoded(MetadataSharedPtr, MutationSharedPtr) {
  // ENVOY_STREAM_LOG(warn, "LocalRateLimit::onMessageEncoded {}", *encoder_callbacks_, metadata->getRequestId());
  return FilterStatus::ContinueIteration;
}

void LocalRateLimit::cleanup() {}

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
