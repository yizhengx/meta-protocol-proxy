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
    :  dispatcher_(dispatcher), rate_limiter_(LocalRateLimiterImpl(
          std::chrono::milliseconds(
              PROTOBUF_GET_MS_OR_DEFAULT(cfg.token_bucket(), fill_interval, 0)),
          cfg.token_bucket().max_tokens(), dispatcher,
          cfg)){
            // ENVOY_LOG(warn, "FilterConfig Constructor");
            // ENVOY_LOG(warn, std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
          }

void LocalRateLimit::onDestroy() { cleanup(); }

void LocalRateLimit::setDecoderFilterCallbacks(DecoderFilterCallbacks& callbacks) {
  callbacks_ = &callbacks;
}

FilterStatus LocalRateLimit::onMessageDecoded(MetadataSharedPtr, MutationSharedPtr) {
  // // Version 1: Original version
  // if (has_buffered) {
  //   // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, resumeIteration {}", *callbacks_, metadata->getRequestId());
  //   return FilterStatus::ContinueIteration;
  // }
  // // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, pauseIteration {}", *callbacks_, metadata->getRequestId());
  // auto it = filter_config_->rateLimiter().getTimeout();
  // std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  // std::chrono::time_point<std::chrono::system_clock> last_timeout = std::get<0>(it);
  // std::chrono::time_point<std::chrono::system_clock> timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  // while (!filter_config_->rateLimiter().setTimeout(timeout, std::get<1>(it))){
  //   it = filter_config_->rateLimiter().getTimeout();
  //   last_timeout = std::get<0>(it);
  //   timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  // }
  // fill_timer_ = filter_config_->dispatcher_.createTimer([this] { onFillTimer(); });
  // ENVOY_LOG(warn, "onMessageDecoded -> Schedule at " + std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(timeout).time_since_epoch().count()));
  // fill_timer_->enableHRTimer(std::chrono::duration_cast<std::chrono::microseconds>(timeout - std::chrono::system_clock::now()));
  // has_buffered = true;
  // return FilterStatus::PauseIteration;

  // // Version 2: Sleep Version
  // // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, pauseIteration {}", *callbacks_, metadata->getRequestId());
  // auto it = filter_config_->rateLimiter().getTimeout();
  // std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  // std::chrono::time_point<std::chrono::system_clock> last_timeout = std::get<0>(it);
  // std::chrono::time_point<std::chrono::system_clock> timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  // while (!filter_config_->rateLimiter().setTimeout(timeout, std::get<1>(it))){
  //   it = filter_config_->rateLimiter().getTimeout();
  //   last_timeout = std::get<0>(it);
  //   timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  // }
  // fill_timer_ = filter_config_->dispatcher_.createTimer([this] { onFillTimer(); });
  // // ENVOY_LOG(warn, "onMessageDecoded -> Schedule at " + std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(timeout).time_since_epoch().count()));
  // std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::microseconds>(timeout - std::chrono::system_clock::now()));
  // return FilterStatus::ContinueIteration;


  // // Version 3: Custom timer version
  // if (has_buffered) {
  //   // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, resumeIteration {}", *callbacks_, metadata->getRequestId());
  //   return FilterStatus::ContinueIteration;
  // }
  // // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, pauseIteration {}", *callbacks_, metadata->getRequestId());
  // auto it = filter_config_->rateLimiter().getTimeout();
  // std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  // std::chrono::time_point<std::chrono::system_clock> last_timeout = std::get<0>(it);
  // std::chrono::time_point<std::chrono::system_clock> timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  // while (!filter_config_->rateLimiter().setTimeout(timeout, std::get<1>(it))){
  //   it = filter_config_->rateLimiter().getTimeout();
  //   last_timeout = std::get<0>(it);
  //   timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  // }
  // // fill_timer_ = filter_config_->dispatcher_.createTimer([this] { onFillTimer(); });
  // ENVOY_LOG(warn, "onMessageDecoded -> Schedule at " + std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(timeout).time_since_epoch().count()));
  // // fill_timer_->enableHRTimer(std::chrono::duration_cast<std::chrono::microseconds>(timeout - std::chrono::system_clock::now()));
  // std::thread timerThread(releaseRequest, std::chrono::duration_cast<std::chrono::microseconds>(timeout - std::chrono::system_clock::now()), callbacks_);
  //   // Detach the timer thread to let it run independently
  // timerThread.detach();
  // has_buffered = true;
  // return FilterStatus::PauseIteration;

  // Version 4: Local timer
  if (has_buffered) {
    // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, resumeIteration {}", *callbacks_, metadata->getRequestId());
    return FilterStatus::ContinueIteration;
  }
  // ENVOY_STREAM_LOG(warn, "meta protocol local rate limit: onMessageDecoded, pauseIteration {}", *callbacks_, metadata->getRequestId());
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  std::chrono::time_point<std::chrono::system_clock> last_timeout = filter_config_->rateLimiter().getTimeout();
  std::chrono::time_point<std::chrono::system_clock> timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  while (!filter_config_->rateLimiter().setTimeout(last_timeout, timeout)){
    // ENVOY_LOG(warn, "Failed, retry");
    last_timeout = filter_config_->rateLimiter().getTimeout();
    timeout = max(last_timeout, now) + filter_config_->rateLimiter().delay;
  }
  fill_timer_ = callbacks_->dispatcher().createTimer([this] { onFillTimer(); });
  ENVOY_LOG(warn, "onMessageDecoded -> Schedule at " + std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(timeout).time_since_epoch().count()));
  fill_timer_->enableTimer(std::chrono::duration_cast<std::chrono::milliseconds>(timeout - std::chrono::system_clock::now()));
  has_buffered = true;
  return FilterStatus::PauseIteration;
}

void LocalRateLimit::releaseRequest(std::chrono::microseconds duration, DecoderFilterCallbacks* callbacs){
  std::this_thread::sleep_for(duration);
  ENVOY_LOG(warn, "onFillTimer "+std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count())) ;
  callbacs->dispatcher().post([=]() {
    ENVOY_LOG(warn, "continueDecoding "+std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count()));
    callbacs->continueDecoding();
  });
}

void LocalRateLimit::onFillTimer(){
  //   ENVOY_LOG(warn, "onFillTimer "+std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count())) ;
  // callbacks_->dispatcher().post([=]() {
  //   ENVOY_LOG(warn, "continueDecoding "+std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count()));
  //   callbacks_->continueDecoding();
  // });

  ENVOY_LOG(warn, "continueDecoding "+std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count()));
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
