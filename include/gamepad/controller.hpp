#pragma once

#include "pros/misc.h"
#include <optional>
#include <string>
#include "button.hpp"
#include "joystick_modifier.hpp"
#include "pros/misc.hpp"

namespace gamepad {
class Gamepad {
    public:
        /**
         * @brief Updates the state of the gamepad (all joysticks and buttons), and also runs
         * any registered listeners.
         *
         * @note This function should be called at the beginning of every loop iteration.
         *
         * @b Example:
         * @code {.cpp}
         * while (true) {
         *   gamepad::master.update();
         *   // do robot control stuff here...
         *   pros::delay(25);
         * }
         * @endcode
         *
         */
        void update();
        /**
         * @brief Get the state of a button on the controller.
         *
         * @param button Which button to return
         *
         * @b Example:
         * @code {.cpp}
         * if(gamepad::master[DIGITAL_L1]) {
         *   // do something here...
         * }
         * @endcode
         *
         */
        const Button& operator[](pros::controller_digital_e_t button);
        /**
         * @brief Get the value of a joystick axis on the controller.
         *
         * @param joystick Which joystick axis to return
         *
         * @b Example:
         * @code {.cpp}
         * // control a motor with a joystick
         * intake.move(gamepad::master[ANALOG_RIGHT_Y]);
         * @endcode
         *
         */
        float operator[](pros::controller_analog_e_t joystick);

        const Button& L1() { return m_L1; }

        const Button& L2() { return m_L2; }

        const Button& R1() { return m_R1; }

        const Button& R2() { return m_R2; }

        const Button& Up() { return m_Up; }

        const Button& Down() { return m_Down; }

        const Button& Left() { return m_Left; }

        const Button& Right() { return m_Right; }

        const Button& X() { return m_X; }

        const Button& B() { return m_B; }

        const Button& Y() { return m_Y; }

        const Button& A() { return m_A; }

        float LeftX(bool use_curve = true) {
            if (use_curve && m_left_transformation) return m_left_transformation->get_value({m_LeftX, m_LeftY}).first;
            else return m_LeftX;
        }

        float LeftY(bool use_curve = true) {
            if (use_curve && m_left_transformation) return m_left_transformation->get_value({m_LeftX, m_LeftY}).second;
            else return m_LeftY;
        }

        float RightX(bool use_curve = true) {
            if (use_curve && m_right_transformation)
                return m_right_transformation->get_value({m_RightX, m_RightY}).first;
            else return m_RightX;
        }

        float RightY(bool use_curve = true) {
            if (use_curve && m_right_transformation)
                return m_right_transformation->get_value({m_RightX, m_RightY}).second;
            else return m_RightY;
        }

        void set_left_transform(Transformation left_transformation) {
            m_left_transformation = std::move(left_transformation);
        }

        void set_right_transform(Transformation right_transformation) {
            m_right_transformation = std::move(right_transformation);
        }

        /// The master controller, same as @ref gamepad::master
        static Gamepad master;
        /// The partner controller, same as @ref gamepad::partner
        static Gamepad partner;
    private:
        Gamepad(pros::controller_id_e_t id)
            : controller(id) {}

        Button m_L1 {}, m_L2 {}, m_R1 {}, m_R2 {}, m_Up {}, m_Down {}, m_Left {}, m_Right {}, m_X {}, m_B {}, m_Y {},
            m_A {};
        float m_LeftX = 0, m_LeftY = 0, m_RightX = 0, m_RightY = 0;
        Button Fake {};
        std::optional<Transformation> m_left_transformation {std::nullopt};
        std::optional<Transformation> m_right_transformation {std::nullopt};
        /**
         * @brief Gets a unique name for a listener that will not conflict with user listener names.
         *
         * @important: when using the function, you must register the listener by
         * directly calling add_listener on the EventHandler, do NOT use onPress/addListener,etc.
         *
         * @return std::string A unique listener name
         */
        static std::string unique_name();
        static Button Gamepad::*button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);
        pros::Controller controller;
};

inline Gamepad Gamepad::master {pros::E_CONTROLLER_MASTER};
inline Gamepad Gamepad::partner {pros::E_CONTROLLER_PARTNER};
/// The master controller
inline Gamepad& master = Gamepad::master;
/// The partner controller
inline Gamepad& partner = Gamepad::partner;

} // namespace gamepad
