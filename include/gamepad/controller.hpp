#pragma once

#include <cstdint>
#include <functional>
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
};

class Button {
        friend class Controller;
    public:
        bool rising_edge = false;
        bool falling_edge = false;
        bool is_pressed = false;
        // uint32_t last_press_time = pros::millis();
        // uint32_t last_release_time = last_press_time;
        uint32_t time_held = 0;
        uint32_t time_released = 0;
        uint32_t long_press_threshold = 500;

        uint32_t onPress(std::function<void(void)> func) const;
        uint32_t onLongPress(std::function<void(void)> func) const;
        uint32_t onRelease(std::function<void(void)> func) const;
        uint32_t addListener(EventType event, std::function<void(void)> func) const;
        bool removeListener(uint32_t id) const;

        explicit operator bool() const { return is_pressed; }
    private:
        void update(bool is_held);

        uint32_t last_update_time = pros::millis();
        mutable EventHandler<> onPressEvent {};
        uint32_t last_long_press_time = 0;
        mutable EventHandler<> onLongPressEvent {};
        mutable EventHandler<> onReleaseEvent {};
};

class Controller {
    public:
        explicit Controller(pros::controller_id_e_t id)
            : controller(id) {}

        /**
         * Updates the state of the gamepad (all joysticks and buttons), and also runs
         * any registered handlers.
         * @note This function should be called at the beginning of every loop iteration.
         * @note Create a separate instance for each task.
         */
        void update();
        /**
         * Get the state of a button on the controller.
         * @param button Which button's state you want.
         */
        const Button& operator[](pros::controller_digital_e_t button);
        /**
         * Get the value of a joystick axis on the controller.
         * @param joystick Which joystick axis's value to return
         */
        float operator[](pros::controller_analog_e_t joystick);
        TODO("hide memebrs and expose getters/const refs")
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
    private:
        Button m_L1 {}, m_L2 {}, m_R1 {}, m_R2 {}, m_Up {}, m_Down {}, m_Left {}, m_Right {}, m_X {}, m_B {}, m_Y {},
            m_A {};
        float m_LeftX = 0, m_LeftY = 0, m_RightX = 0, m_RightY = 0;
        Button Fake {};
        static Button Controller::*button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);
        pros::Controller controller;
}; // namespace Gamepad
} // namespace Gamepad