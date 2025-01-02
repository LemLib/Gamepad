#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "event_handler.hpp"

namespace gamepad {
enum EventType {
    ON_PRESS,
    ON_LONG_PRESS,
    ON_RELEASE,
    ON_SHORT_RELEASE,
    ON_LONG_RELEASE,
    ON_REPEAT_PRESS,
};

class Button {
        friend class Gamepad;
    public:
        /// Whether the button has just been pressed
        bool rising_edge = false;
        /// Whether the button has just been released
        bool falling_edge = false;
        /// Whether the button is currently held down
        bool is_pressed = false;
        /// How long the button has been held down
        uint32_t time_held = 0;
        /// How long the button has been released
        uint32_t time_released = 0;
        /// How many times the button has been repeat-pressed
        uint32_t repeat_iterations = 0;
        /**
         * @brief Set the time for a press to be considered a long press for the button
         *
         * @note this is likely to be used with the onLongPress(), onShortRelease(), onLongRelease(), or onRepeatPress()
         * events
         *
         * @param threshold the time in ms that would be considered a long press
         *
         * @b Example:
         * @code {.cpp}
         *   // change the threshold
         *   gamepad::master.Left.setLongPressThreshold(5000);
         *   // then call the function
         *   gamepad::master.Left.onLongPress("longPress1", []() {
         *       std::cout << "I was held for 5000ms instead of the 500ms default!" << std::endl;
         *   });
         * @endcode
         */
        void setLongPressThreshold(uint32_t threshold) const;
        /**
         * @brief Set the interval for the repeatPress event to repeat
         *
         * @note this is likely to be used with the onRepeatPress() event
         *
         * @param cooldown the interval in ms
         *
         * @b Example:
         * @code {.cpp}
         *   // change the threshold
         *   gamepad::master.Up.setRepeatCooldown(100);
         *   // then call the function
         *   gamepad::master.Up.onRepeatPress("repeatPress1", []() {
         *       std::cout << "I'm being repeated every 100ms instead of the 50ms default!" << std::endl;
         *   });
         * @endcode
         */
        void setRepeatCooldown(uint32_t cooldown) const;
        /**
         * @brief Register a function to run when the button is pressed.
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run when the button is pressed, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.Down.onPress("downPress1", downPress1);
         *   // ...or a lambda
         *   gamepad::master.Up.onPress("upPress1", []() { std::cout << "I was pressed!" << std::endl; });
         * @endcode
         */
        bool onPress(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run when the button is long pressed.
         *
         * By default, onLongPress will fire when the button has been held down for
         * 500ms or more, this threshold can be adjusted via the setLongPressThreshold() method.
         *
         * @warning When using this event along with onPress, both the onPress
         * and onLongPress listeners may fire together.
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run when the button is long pressed, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.Left.onLongPress("fireCatapult", fireCatapult);
         *   // ...or a lambda
         *   gamepad::master.Right.onLongPress("print_right", []() { std::cout << "Right button was long pressed!" <<
         * std::endl; });
         * @endcode
         */
        bool onLongPress(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run when the button is released.
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run when the button is released, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.X.onRelease("stopFlywheel", stopFlywheel);
         *   // ...or a lambda
         *   gamepad::master.Y.onRelease("stopIntake", []() { intake.move(0); });
         * @endcode
         */
        bool onRelease(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run when the button is short released.
         *
         * By default, shortRelease will fire when the button has been released before 500ms, this threshold can be
         * adjusted via the setLongPressThreshold() method.
         *
         * @note This event will most likely be used along with the longPress event.
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run when the button is short released, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.A.onShortRelease("raiseLiftOneLevel", raiseLiftOneLevel);
         *   // ...or a lambda
         *   gamepad::master.B.onShortRelease("intakeOnePiece", []() { intake.move_relative(600, 100); });
         * @endcode
         */
        bool onShortRelease(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run when the button is long released.
         *
         * By default, longRelease will fire when the button has been released after 500ms, this threshold can be
         * adjusted via the setLongPressThreshold() method.
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run when the button is long released, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.Up.onLongRelease("moveLiftToGround", moveLiftToGround);
         *   // ...or a lambda
         *   gamepad::master.Left.onLongRelease("spinIntake", []() { intake.move(127); });
         * @endcode
         *
         */
        bool onLongRelease(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run periodically after its been held
         *
         * By default repeatPress will start repeating after 500ms and repeat every 50ms, this can be adjusted via the
         * setLongPressThreshold() and setRepeatCooldown() methods respectively
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func the function to run periodically when the button is held, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.X.onRepeatPress("shootDisk", shootOneDisk);
         *   // ...or a lambda
         *   gamepad::master.A.onRepeatPress("scoreOneRing", []() { intake.move_relative(200, 100); });
         * @endcode
         *
         */
        bool onRepeatPress(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run for a given event.
         *
         * @param event Which event to register the listener on.
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run for the given event, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   gamepad::master.L1.addListener(gamepad::ON_PRESS, "start_spin", startSpin);
         *   // ...or a lambda
         *   gamepad::master.L1.addListener(gamepad::ON_RELEASE, "stop_spin", []() { motor1.brake(); });
         * @endcode
         */
        bool addListener(EventType event, std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Removes a listener from the button
         * @warning Usage of this function is discouraged.
         *
         * @param listenerName The name of the listener to remove
         * @return true The specified listener was successfully removed
         * @return false The specified listener could not be removed
         *
         * @b Example:
         * @code {.cpp}
         *   // Add an event listener...
         *   gamepad::master.L1.addListener(gamepad::ON_PRESS, "do_something", doSomething);
         *   // ...and now get rid of it
         *   gamepad::master.L1.removeListener("do_something");
         * @endcode
         */
        bool removeListener(std::string listenerName) const;

        /**
         * @brief Returns a value indicating whether the button is currently being held.
         *
         * @return true The button is currently pressed
         * @return false The button is not currently pressed
         */
        explicit operator bool() const { return is_pressed; }
    private:
        /**
         * @brief Updates the button and runs any event handlers, if necessary
         *
         * @param is_held Whether or not the button is currently held down
         */
        void update(bool is_held);
        /// How long the threshold should be for the longPress and shortRelease events
        mutable uint32_t m_long_press_threshold = 500;
        /// How often repeatPress is called
        mutable uint32_t m_repeat_cooldown = 50;
        /// The last time the update function was called
        uint32_t m_last_update_time = pros::millis();
        /// The last time the long press event was fired
        uint32_t m_last_long_press_time = 0;
        /// The last time the repeat event was called
        uint32_t m_last_repeat_time = 0;
        mutable _impl::EventHandler<std::string> m_on_press_event {};
        mutable _impl::EventHandler<std::string> m_on_long_press_event {};
        mutable _impl::EventHandler<std::string> m_on_release_event {};
        mutable _impl::EventHandler<std::string> m_on_short_release_event {};
        mutable _impl::EventHandler<std::string> m_on_long_release_event {};
        mutable _impl::EventHandler<std::string> m_on_repeat_press_event {};
};
} // namespace gamepad