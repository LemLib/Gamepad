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
        const ButtonState& operator[](pros::controller_digital_e_t button);
        /**
         * Get the value of a joystick axis on the controller.
         * @param joystick Which joystick axis's value to return
         */
        float operator[](pros::controller_analog_e_t joystick);
        enum EventType {
            ON_CLICK,
            ON_RELEASE,
            ON_LONG_CLICK
        };
        /**
         * Register a function to be run when a specified event occurs on a button.
         * @param button The button to monitor
         * @param event The type of event
         * @param func The function to register
         * @returns An id number that can be passed to removeHandler
         */
        uint32_t addHandler(pros::controller_digital_e_t button, EventType event, std::function<void(void)> func);
        /**
         * Un-register a function that was registered with addHandler
         * @param id The id of the handler to be removed
         * @returns Whether the handler was removed successfully
         */
        bool removeHandler(uint32_t id);
        ButtonState L1{}, L2{}, R1{}, R2{}, 
        Up{}, Down{}, Left{}, Right{}, 
        X{}, B{}, Y{}, A{};
        float LeftX = 0, LeftY = 0, RightX = 0, RightY = 0;
    private:
        static ButtonState Controller::* button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(ButtonState Controller::* button, bool is_held);
        pros::Controller controller;
        std::map<std::pair<ButtonState Controller::*, EventType>, EventHandler<void>> button_events;
}; // namespace Gamepad
}