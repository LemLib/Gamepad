#pragma once

#include <mutex>
#include <functional>
#include <vector>
#include <algorithm>

#include "gamepad/recursive_mutex.hpp"

namespace gamepad::_impl {

<<<<<<< HEAD
class MonotonicCounter {
        template <typename... Args> friend class EventHandler;

        static uint32_t next_value() {
            static std::atomic<uint32_t> counter = 0;
            return ++counter;
        }
};

template <typename... Args> class EventHandler {
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
            if (listeners.find(id) == listeners.end()) {
                TODO("change handling maybe?")
                return false;
            }
            listeners.erase(id);
            return true;
        }

=======
/**
 * @brief Event handling class with thread safety that supports adding, removing, and running listeners
 *
 * @tparam Key the key type for (un)registering listener (this type MUST support operator== and operator!=)
 * @tparam Args the types of the parameters that each listener is passed
 */
template <typename Key, typename... Args> class EventHandler {
    public:
        using Listener = std::function<void(Args...)>;

        /**
         * @brief Add a listener to the list of listeners
         *
         * @param key The listener key (this must be a unique key value)
         * @param func The function to run when this event is fired
         * @return true The listener was successfully added
         * @return false The listener was NOT successfully added (there is already a listener with the same key)
         */
        bool add_listener(Key key, Listener func) {
            std::lock_guard lock(mutex);
            if (std::find(keys.begin(), keys.end(), key) != keys.end()) return false;
            keys.push_back(key);
            listeners.push_back(func);
            return true;
        }

        /**
         * @brief Remove a listener from the list of listeners
         *
         * @param key The listener key (this must be a unique key value)
         * @return true The listener was successfully removed
         * @return false The listener was NOT successfully removed (there is no listener with the same key)
         */
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

        /**
         * @brief Whther or not there are any listeners registered
         *
         * @return true There are listeners registered
         * @return false There are no listeners registered
         */
>>>>>>> b19f6224e83dcbbf6f3b02059fa6d64b57af4ac8
        bool is_empty() {
            std::lock_guard lock(mutex);
            return listeners.empty();
        }

<<<<<<< HEAD
        void fire(Args... args) {
            std::lock_guard lock(mutex);
            for (auto listener : listeners) { listener.second(args...); }
        }
    private:
        std::map<uint32_t, Listener> listeners;
        pros::Mutex mutex;
};
} // namespace Gamepad
=======
        /**
         * @brief Runs each listener registered
         *
         * @param args The parameters to pass to each listener
         */
        void fire(Args... args) {
            std::lock_guard lock(mutex);
            for (auto listener : listeners) { listener(args...); }
        }
    private:
        std::vector<Key> keys {};
        std::vector<Listener> listeners {};
        gamepad::_impl::RecursiveMutex mutex {};
};
} // namespace gamepad::_impl
>>>>>>> b19f6224e83dcbbf6f3b02059fa6d64b57af4ac8
