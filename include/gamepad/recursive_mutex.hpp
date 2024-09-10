#include "pros/apix.h"
#include "pros/rtos.h"

namespace Gamepad::_impl {

class RecursiveMutex {
    public:
        /**
         * @brief Construct a new recursive mutex
         *
         */
        RecursiveMutex()
            : mutex(pros::c::mutex_recursive_create()) {}

        /**
         * @brief Locks the recursive mutex, optionally bailing out after a timeout
         *
         * @param timeout How long to wait for the mutex before baling out
         * @return true The mutex was successfully acquired
         * @return false The mutex was not successfully acquired
         */
        bool take(std::uint32_t timeout = TIMEOUT_MAX) { return pros::c::mutex_recursive_take(mutex, timeout); }

        /**
         * @brief Locks the mutex, waiting indefinetely until the mutex is acquired
         *
         */
        void lock() {
            while (!this->take()) pros::delay(2);
        }

        /**
         * @brief Attempts to lock the mutex without blocking the current thread
         *
         * @return true The mutex was successfully acquired
         * @return false The mutex was not successfully acquired
         */
        bool try_lock() { return this->take(0); }

        /**
         * @brief Unlocks the mutex
         *
         * @return true The mutex was successfully released
         * @return false The mutex was not successfully released
         */
        bool give() { return pros::c::mutex_recursive_give(mutex); }

        /**
         * @brief Unlocks the mutex, equivalent to \ref give()
         *
         */
        void unlock() { this->give(); }

        /**
         * @brief Destroy the recursive mutex and free any allocated memory
         */
        ~RecursiveMutex() { pros::c::mutex_delete(mutex); }
    private:
        pros::mutex_t mutex;
};

} // namespace Gamepad::_impl