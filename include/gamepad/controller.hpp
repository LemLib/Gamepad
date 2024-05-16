#pragma once

#include <cstdint>
#ifndef PROS_USE_SIMPLE_NAMES
#define PROS_USE_SIMPLE_NAMES
#endif

#include "pros/misc.hpp"
#include "pros/rtos.hpp"

namespace Gamepad {
struct ButtonState {
    bool rising_edge = false;
    bool falling_edge = false;
    bool is_pressed = false;
    uint32_t last_press_time = pros::millis();
    uint32_t last_release_time = last_press_time;
};

class Controller {
    public:
        explicit Controller(pros::controller_id_e_t id): controller(id) {}
        /**
         * Updates the state of the gamepad (all joysticks and buttons).
         * @note This function should be called at the beginning of every loop iteration.
         * @note Create a separate instance for each task.
         */
        void update();
        ButtonState operator[](pros::controller_digital_e_t button);
        float operator[](pros::controller_analog_e_t button);
        ButtonState L1{}, L2{}, R1{}, R2{}, 
        Up{}, Down{}, Left{}, Right{}, 
        X{}, B{}, Y{}, A{};
        float LeftX = 0, LeftY = 0, RightX = 0, RightY = 0;
    private:
        pros::Controller controller;
        void updateButton(ButtonState Controller::* button, bool is_held);
}; // namespace Gamepad
}