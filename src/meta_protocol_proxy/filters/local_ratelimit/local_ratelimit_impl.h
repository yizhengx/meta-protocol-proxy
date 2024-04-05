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

struct QueueElement{
  DecoderFilterCallbacks* callbacks;
  std::chrono::time_point<std::chrono::system_clock> timeout;
};

class TSQueue: public Logger::Loggable<Logger::Id::filter>{
private:
    // Underlying queue
    std::queue<QueueElement> queue_;
    // mutex for thread synchronization
    mutable std::mutex mutex_;
    std::chrono::time_point<std::chrono::system_clock> last_timeout;
    std::chrono::microseconds delay;
  
public:
    TSQueue(std::chrono::microseconds delay_){
      delay = delay_;
    }

    // Pushes an element to the queue
    void push(DecoderFilterCallbacks* callbacks)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (queue_.empty()){
        last_timeout = std::chrono::system_clock::now();
        QueueElement ele = {callbacks, std::chrono::time_point(last_timeout)};
        queue_.push(ele);
        ENVOY_STREAM_LOG(warn, "Push a request to empty queue", *callbacks);
      }else{
        last_timeout += delay;
        QueueElement ele = {callbacks, std::chrono::time_point(last_timeout)};
        queue_.push(ele);
        ENVOY_STREAM_LOG(warn, "Push a request to a non empty queue", *callbacks, queue_.size());
      }
    }
  
    // Pops an element off the queue
    void pop()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      while (!queue_.empty())
      {
        QueueElement ele = queue_.front();
        std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
        if (ele.timeout < t){
          ENVOY_LOG(warn, "should resume request");
          ele.callbacks->continueDecoding();
          queue_.pop();
          ENVOY_LOG(warn, "resume request");
        }else{
          break;
        }
      }
    }
};

class LocalRateLimiterImpl: public Logger::Loggable<Logger::Id::filter> {
public:
  LocalRateLimiterImpl(
      const std::chrono::milliseconds fill_interval, const uint32_t max_tokens,
      Event::Dispatcher& dispatcher, const LocalRateLimitConfig& cfg);
  ~LocalRateLimiterImpl();

  // Custom Logic
  void bufferRequest(DecoderFilterCallbacks* callbacks);

private:
  void onFillTimer();
  void onFillTimerCostomHelper();

  const Event::TimerPtr fill_timer_;
  TimeSource& time_source_;
  std::chrono::milliseconds timer_duration_;

  LocalRateLimitConfig config_;
  TSQueue queue;
};

} // namespace LocalRateLimit
} // namespace MetaProtocolProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
