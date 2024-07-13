#pragma once

#include <mutex>
#include <functional>
#include <vector>
#include <algorithm>

#include "gamepad/recursive_mutex.hpp"

namespace Gamepad {

template <typename Key, typename... Args> class EventHandler {
    public:
        using Listener = std::function<void(Args...)>;

        bool add_listener(Key key, Listener func) {
            std::lock_guard lock(mutex);
            if (std::find(keys.begin(), keys.end(), key) != keys.end()) return false;
            keys.push_back(key);
            listeners.push_back(func);
            return true;
        }

        bool remove_listener(Key key) {
            std::lock_guard lock(mutex);
            auto i = std::find(keys.begin(), keys.end(), key);
            if (i != keys.end()) {
                keys.erase(i);
                listeners.erase(listeners.begin() + (i - keys.begin()));
                return true;
            }
            return false;
        }

        bool is_empty() {
            std::lock_guard lock(mutex);
            return listeners.empty();
        }

        void fire(Args... args) {
            std::lock_guard lock(mutex);
            for (auto listener : listeners) { listener(args...); }
        }
    private:
        std::vector<Key> keys {};
        std::vector<Listener> listeners {};
        Gamepad::RecursiveMutex mutex {};
};
} // namespace Gamepad