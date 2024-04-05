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

FilterConfig::FilterConfig(const LocalRateLimitConfig& cfg, Stats::Scope& scope,
                           Event::Dispatcher& dispatcher)
    : stats_(LocalRateLimitStats::generateStats(cfg.stat_prefix(), scope)),
      rate_limiter_(LocalRateLimiterImpl(
          std::chrono::milliseconds(
              PROTOBUF_GET_MS_OR_DEFAULT(cfg.token_bucket(), fill_interval, 0)),
          cfg.token_bucket().max_tokens(), dispatcher,
          cfg)) {
            ENVOY_LOG(warn, "FilterConfig Constructor");
          }

void LocalRateLimit::onDestroy() { cleanup(); }

void LocalRateLimit::setDecoderFilterCallbacks(DecoderFilterCallbacks& callbacks) {
  callbacks_ = &callbacks;
}

FilterStatus LocalRateLimit::onMessageDecoded(MetadataSharedPtr metadata, MutationSharedPtr) {
  if (has_buffered) {
    ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, resumeIteration {}", *callbacks_, metadata->getRequestId());
    return FilterStatus::ContinueIteration;
  }
  ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, pauseIteration {}", *callbacks_, metadata->getRequestId());
  has_buffered = true;
  filter_config_->rateLimiter().bufferRequest(callbacks_);
  return FilterStatus::PauseIteration;
}

void LocalRateLimit::setEncoderFilterCallbacks(EncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
}

FilterStatus LocalRateLimit::onMessageEncoded(MetadataSharedPtr metadata, MutationSharedPtr) {
  ENVOY_STREAM_LOG(warn, "LocalRateLimit::onMessageEncoded {}", *encoder_callbacks_, metadata->getRequestId());
  return FilterStatus::ContinueIteration;
}

void LocalRateLimit::cleanup() {}

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
