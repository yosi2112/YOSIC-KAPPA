#pragma once
#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>
namespace yosic {
class Clock {
    struct Event {
        std::uint64_t tick, sequence;
        std::function<void()> callback;
    };
    struct Later {
        bool operator()(const Event &a, const Event &b) const {
            return a.tick > b.tick || (a.tick == b.tick && a.sequence > b.sequence);
        }
    };
    std::priority_queue<Event, std::vector<Event>, Later> events_;
    std::uint64_t now_ = 0, next_ = 0;

  public:
    std::uint64_t now() const { return now_; }
    void schedule_at(std::uint64_t tick, std::function<void()> callback) {
        if (tick < now_ || !callback)
            throw std::invalid_argument("invalid event");
        if (next_ == std::numeric_limits<std::uint64_t>::max())
            throw std::overflow_error("event sequence overflow");
        events_.push({tick, next_++, std::move(callback)});
    }
    void advance(std::uint64_t target) {
        if (target < now_)
            throw std::invalid_argument("clock cannot run backwards");
        std::size_t dispatched = 0;
        while (!events_.empty() && events_.top().tick <= target) {
            if (++dispatched > 1000000)
                throw std::runtime_error("event dispatch limit exceeded");
            auto event = events_.top();
            events_.pop();
            now_ = event.tick;
            event.callback();
        }
        now_ = target;
    }
};
} // namespace yosic
