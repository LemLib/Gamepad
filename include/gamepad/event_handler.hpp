#pragma once

#include <mutex>
#include <utility>
#include <functional>
#include <map>
#include <atomic>

#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"

namespace Gamepad {

class MonotonicCounter {
    template<typename... Args> friend class EventHandler;
    static uint32_t next_value() {
        static std::atomic<uint32_t> counter = 0;
        return ++counter;
    }
};

template <typename... Args>
class EventHandler {
    public:
    using Listener = std::function<void(Args...)>;
    uint32_t add_listener(Listener func) {
        std::lock_guard lock(mutex);
        uint32_t id = MonotonicCounter::next_value();
        listeners.emplace(id, std::move(func));
        return id;
    }
    bool remove_listener(uint32_t id) {
        std::lock_guard lock(mutex);
        if(listeners.find(id) == listeners.end()) {
            TODO("change handling maybe?")
            return false;
        }
        listeners.erase(id);
        return true;
    }
    bool is_empty() {
        std::lock_guard lock(mutex);
        return listeners.empty();
    }
    void fire(Args... args) {
        std::lock_guard lock(mutex);
        for(auto listener : listeners) {
            listener.second(args...);
        }
    }
    private:
    std::map<uint32_t, Listener> listeners;
    pros::Mutex mutex;
};
}