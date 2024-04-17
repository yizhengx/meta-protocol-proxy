#pragma once

#include <memory>
#include <string>
#include <thread>

#include "envoy/network/filter.h"
#include "envoy/stats/scope.h"
#include "source/common/common/logger.h"
#include "envoy/stats/stats_macros.h"
#include "envoy/ratelimit/ratelimit.h"
#include "envoy/event/timer.h"

#include "api/meta_protocol_proxy/filters/local_ratelimit/v1alpha/local_ratelimit.pb.h"

#include "src/meta_protocol_proxy/filters/filter.h"

#include "src/meta_protocol_proxy/filters/local_ratelimit/local_ratelimit_impl.h"
#include "src/meta_protocol_proxy/filters/local_ratelimit/stats.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace MetaProtocolProxy {
namespace LocalRateLimit {

class FilterConfig : public Logger::Loggable<Logger::Id::filter> {
public:
  FilterConfig(const LocalRateLimitConfig& cfg, Stats::Scope& scope, Event::Dispatcher& dispatcher);
  ~FilterConfig() = default;

  LocalRateLimiterImpl& rateLimiter() { return rate_limiter_; }

  Event::Dispatcher& dispatcher_;

private:
  LocalRateLimiterImpl rate_limiter_;
};

class LocalRateLimit : public CodecFilter, Logger::Loggable<Logger::Id::filter> {
public:
  LocalRateLimit(std::shared_ptr<FilterConfig> filter_config) : filter_config_(filter_config){
    has_buffered = false;
    // ENVOY_LOG(warn, "onLocalRateLimit Constructor " + std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count()));

  };
  ~LocalRateLimit() override = default;

  void onDestroy() override;

  // DecoderFilter
  void setDecoderFilterCallbacks(DecoderFilterCallbacks& callbacks) override;
  FilterStatus onMessageDecoded(MetadataSharedPtr metadata, MutationSharedPtr mutation) override;

  void setEncoderFilterCallbacks(EncoderFilterCallbacks& callbacks) override;
  FilterStatus onMessageEncoded(MetadataSharedPtr, MutationSharedPtr) override;


  static void releaseRequest(std::chrono::microseconds, DecoderFilterCallbacks*);

  void onFillTimer();

private:
  void cleanup();

  Event::TimerPtr fill_timer_;

  DecoderFilterCallbacks* callbacks_{};
  EncoderFilterCallbacks* encoder_callbacks_{};

  std::shared_ptr<FilterConfig> filter_config_;
  bool has_buffered;
};

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
