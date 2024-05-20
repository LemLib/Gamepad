#pragma once

#include <cstdint>
#include <map>
#include <utility>
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
    public:
    bool rising_edge = false;
    bool falling_edge = false;
    bool is_pressed = false;
    uint32_t last_press_time = pros::millis();
    uint32_t last_release_time = last_press_time;

    uint32_t onPress(std::function<void(void)> func);
    uint32_t onLongPress(std::function<void(void)> func);
    uint32_t onRelease(std::function<void(void)> func);
    uint32_t addListener(EventType event, std::function<void(void)> func);
    bool removeListener(uint32_t id);
    private:
    EventHandler<> onPressEvent;
    EventHandler<> onLongPressEvent;
    EventHandler<> onReleaseEvent;
};

class Controller {
    public:
        explicit Controller(pros::controller_id_e_t id): controller(id) {}
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
        Button L1{}, L2{}, R1{}, R2{}, 
        Up{}, Down{}, Left{}, Right{}, 
        X{}, B{}, Y{}, A{};
        float LeftX = 0, LeftY = 0, RightX = 0, RightY = 0;
    private:
        static Button Controller::* button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);
        pros::Controller controller;
}; // namespace Gamepad
}