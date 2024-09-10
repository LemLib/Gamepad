#include "event_handler.hpp"
#include "pros/rtos.hpp"
#include <functional>
#include <string>

namespace Gamepad {

enum EventType {
    ON_PRESS,
    ON_LONG_PRESS,
    ON_RELEASE,
    ON_SHORT_RELEASE,
};

class Button {
        friend class Controller;
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
        /// How long the threshold should be for the longPress and shortRelease events
        uint32_t long_press_threshold = 500;
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
         *   Gamepad::master.Down.onPress("downPress1", downPress1);
         *   // ...or a lambda
         *   Gamepad::master.Up.onPress("upPress1", []() { std::cout << "I was pressed!" << std::endl; });
         * @endcode
         */
        bool onPress(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run when the button is long pressed.
         *
         * By default, onLongPress will fire when the button has been held down for
         * 500ms or more, this threshold can be adjusted by changing long_press_threshold.
         *
         * @warning When using this event along with onPress, both the onPress
         * and onlongPress listeners may fire together.
         *
         * @param listenerName The name of the listener, this must be a unique name
         * @param func The function to run when the button is long pressed, the function MUST NOT block
         * @return true The listener was successfully registered
         * @return false The listener was not successfully registered (there is already a listener with this name)
         *
         * @b Example:
         * @code {.cpp}
         *   // Use a function...
         *   Gamepad::master.Left.onLongPress("fireCatapult", fireCatapult);
         *   // ...or a lambda
         *   Gamepad::master.Right.onLongPress("print_right", []() { std::cout << "Right button was long pressed!" <<
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
         *   Gamepad::master.X.onRelease("stopFlywheel", stopFlywheel);
         *   // ...or a lambda
         *   Gamepad::master.Y.onRelease("stopIntake", []() { intake.move(0); });
         * @endcode
         */
        bool onRelease(std::string listenerName, std::function<void(void)> func) const;
        /**
         * @brief Register a function to run when the button is short released.
         *
         * By default, shortRelease will fire when the button has been released before 500ms, this threshold can be
         * adjusted by changing long_press_threshold.
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
         *   Gamepad::master.A.onShortRelease("raiseLiftOneLevel", raiseLiftOneLevel);
         *   // ...or a lambda
         *   Gamepad::master.B.onShortRelease("intakeOnePicce", []() { intake.move_relative(600, 100); });
         * @endcode
         */
        bool onShortRelease(std::string listenerName, std::function<void(void)> func) const;
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
         *   Gamepad::master.L1.addListener(Gamepad::ON_PRESS, "start_spin", startSpin);
         *   // ...or a lambda
         *   Gamepad::master.L1.addListener(Gamepad::ON_RELEASE, "stop_spin", []() { motor1.brake(); });
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
         *   Gamepad::master.L1.addListener(Gamepad::ON_PRESS, "do_something", doSomething);
         *   // ...and now get rid of it
         *   Gamepad::master.L1.removeListener("do_something");
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
        /// he last time the update function was called
        uint32_t last_update_time = pros::millis();
        /// The last time the long press event was fired
        uint32_t last_long_press_time = 0;
        mutable _impl::EventHandler<std::string> onPressEvent {};
        mutable _impl::EventHandler<std::string> onLongPressEvent {};
        mutable _impl::EventHandler<std::string> onReleaseEvent {};
        mutable _impl::EventHandler<std::string> onShortReleaseEvent {};
};
} // namespace Gamepad