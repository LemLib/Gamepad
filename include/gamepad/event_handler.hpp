#pragma once

#include <mutex>
#include <functional>
#include <vector>
#include <algorithm>

#include "gamepad/recursive_mutex.hpp"

namespace gamepad::_impl {

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
         * @return 0 The listener was successfully added
         * @return INT32_MAX The listener was NOT successfully added (there is already a listener with the same key)
         */
        int32_t addListener(Key key, Listener func) {
            std::lock_guard lock(m_mutex);
            if (std::find(m_keys.begin(), m_keys.end(), key) != m_keys.end()) return INT32_MAX;
            m_keys.push_back(key);
            m_listeners.push_back(func);
            return 0;
        }

        /**
         * @brief Remove a listener from the list of listeners
         *
         * @param key The listener key (this must be a unique key value)
         * @return 0 The listener was successfully removed
         * @return INT32_MAX The listener was NOT successfully removed (there is no listener with the same key)
         */
        int32_t removeListener(Key key) {
            std::lock_guard lock(m_mutex);
            auto i = std::find(m_keys.begin(), m_keys.end(), key);
            if (i != m_keys.end()) {
                m_keys.erase(i);
                m_listeners.erase(m_listeners.begin() + (i - m_keys.begin()));
                return 0;
            }
            return INT32_MAX;
        }

        /**
         * @brief Whether or not there are any listeners registered
         *
         * @return true There are listeners registered
         * @return false There are no listeners registered
         */
        bool isEmpty() {
            std::lock_guard lock(m_mutex);
            return m_listeners.empty();
        }

        /**
         * @brief Runs each listener registered
         *
         * @param args The parameters to pass to each listener
         */
        void fire(Args... args) {
            std::lock_guard lock(m_mutex);
            for (auto listener : m_listeners) { listener(args...); }
        }
    private:
        std::vector<Key> m_keys {};
        std::vector<Listener> m_listeners {};
        gamepad::_impl::RecursiveMutex m_mutex {};
};
} // namespace gamepad::_impl
