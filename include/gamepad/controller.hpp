#pragma once

#include "pros/misc.h"
#include <cstdint>
#include <functional>
#include <string>
#ifndef PROS_USE_SIMPLE_NAMES
#define PROS_USE_SIMPLE_NAMES
#endif

#include "event_handler.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.hpp"

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
         * By default, onLongPress will fire when the button has been held down for
         * 500ms or more, this threshold can be adjusted by changing long_press_threshold.
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
         * By default, shortRelease will fire when the button has been released before 500ms, this threshold can be
         * adjusted by changing long_press_threshold.
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

class Controller {
    public:
        /**
         * Updates the state of the gamepad (all joysticks and buttons), and also runs
         * any registered handlers.
         * @note This function should be called at the beginning of every loop iteration.
         *
         * @b Example:
         * @code {.cpp}
         * while (true) {
         *   Gamepad::master.update();
         *   // do robot control stuff here...
         *   pros::delay(25);
         * }
         * @endcode
         *
         */
        void update();
        /**
         * Get the state of a button on the controller.
         * @param button Which button to return
         *
         * @b Example:
         * @code {.cpp}
         * if(Gamepad::master[DIGITAL_L1]) {
         *   // do something here...
         * }
         * @endcode
         *
         */
        const Button& operator[](pros::controller_digital_e_t button);
        /**
         * Get the value of a joystick axis on the controller.
         * @param joystick Which joystick axis to return
         *
         * @b Example:
         * @code {.cpp}
         * // control a motor with a joystick
         * intake.move(Gamepad::master[ANALOG_RIGHT_Y]);
         * @endcode
         *
         */
        float operator[](pros::controller_analog_e_t joystick);
        const Button& L1 {m_L1};
        const Button& L2 {m_L2};
        const Button& R1 {m_R1};
        const Button& R2 {m_R2};
        const Button& Up {m_Up};
        const Button& Down {m_Down};
        const Button& Left {m_Left};
        const Button& Right {m_Right};
        const Button& X {m_X};
        const Button& B {m_B};
        const Button& Y {m_Y};
        const Button& A {m_Down};
        const float& LeftX = m_LeftX;
        const float& LeftY = m_LeftY;
        const float& RightX = m_RightX;
        const float& RightY = m_RightY;
        /// The master controller, same as @ref Gamepad::master
        static Controller master;
        /// The partner controller, same as @ref Gamepad::partner
        static Controller partner;
    private:
        Controller(pros::controller_id_e_t id)
            : controller(id) {}

        Button m_L1 {}, m_L2 {}, m_R1 {}, m_R2 {}, m_Up {}, m_Down {}, m_Left {}, m_Right {}, m_X {}, m_B {}, m_Y {},
            m_A {};
        float m_LeftX = 0, m_LeftY = 0, m_RightX = 0, m_RightY = 0;
        Button Fake {};
        /**
         * @brief Gets a unique name for a listener that will not conflict with user listener names. IMPORTANT: when
         * using the function, you must register the listener by directly calling add_listener on the EventHandler, do
         * NOT use onPress/addListener,etc.
         *
         * @return std::string A unique listener name
         */
        static std::string unique_name();
        static Button Controller::*button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);
        pros::Controller controller;
};

inline Controller Controller::master {pros::E_CONTROLLER_MASTER};
inline Controller Controller::partner {pros::E_CONTROLLER_PARTNER};
/// The master controller
inline Controller& master = Controller::master;
/// The partner controller
inline Controller& partner = Controller::partner;

} // namespace Gamepad
