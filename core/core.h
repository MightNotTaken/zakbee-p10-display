#ifndef CORE_H__
#define CORE_H__
#include <map>
#include <functional>
#include "interval.h"
#include "timeout.h"
#include "utility.h"

namespace Core {
  std::map<IntervalReference, Interval *> intervals;
  std::map<TimeoutReference, Timeout *> timeouts;

  IntervalReference &setInterval(std::function<void()> callback, IntervalDuration duration) {
    Interval *interval = new Interval(callback, duration);
    Core::intervals[interval->getID()] = interval;
    return interval->getID();
  }

  IntervalReference &setImmediate(std::function<void()> callback, IntervalDuration duration) {
    callback();
    return setInterval(callback, duration);
  }

  TimeoutReference &setTimeout(std::function<void()> callback, IntervalDuration duration) {
    Timeout *timeout = new Timeout(callback, duration);
    Core::timeouts[timeout->getID()] = timeout;
    return timeout->getID();
  }

  void clearInterval(IntervalReference &ref) {
    auto it = Core::intervals.find(ref);
    if (it != Core::intervals.end()) {
      delete it->second;
      Core::intervals.erase(it);
    }
    ref = NULL_REFERENCE;
  }

  void execute(std::function<void()> executable) {
    invoke(executable);
  }

  void clearImmediate(IntervalReference &ref) {
    clearInterval(ref);
  }

  void clearTimeout(TimeoutReference &ref) {
    auto it = Core::timeouts.find(ref);
    if (it != Core::timeouts.end()) {
      delete it->second;
      Core::timeouts.erase(it);
    }
    ref = NULL_REFERENCE;
  }

  void begin(uint32_t intervalSize = MINUTES(1)) {
    setInterval([]() {
      console.log("uptime: ");
      console.log(formatMillis(millis()));
    }, intervalSize);
  }

  void loop() {

    for (auto [_, interval] : Core::intervals) {
      interval->execute();
    }

    for (auto [_, timeout] : Core::timeouts) {
      if (timeout->execute()) {
        clearTimeout(timeout->getID());
      }
    }
  }
};

#endif