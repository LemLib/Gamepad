#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <string>
#include <sys/types.h>
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
    uint32_t last_press_time = pros::millis();
    uint32_t last_release_time = last_press_time;
    uint32_t time_held = 0;
    uint32_t time_released = 0;
    uint32_t long_press_threshold = 500;

    uint32_t onPress(std::function<void(void)> func);
    uint32_t onLongPress(std::function<void(void)> func);
    uint32_t onRelease(std::function<void(void)> func);
    uint32_t addListener(EventType event, std::function<void(void)> func);
    bool removeListener(uint32_t id);
    private:

    void update(bool is_held);

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

        void print_line(uint8_t line, std::string str, uint32_t duration);

        void rumble(std::string rumble_pattern);
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

        TODO("hide members and expose getters/const refs")

        Button L1{}, L2{}, R1{}, R2{}, 
        Up{}, Down{}, Left{}, Right{}, 
        X{}, B{}, Y{}, A{};
        float LeftX = 0, LeftY = 0, RightX = 0, RightY = 0;
    private:
        struct Line {
            std::string text;
            uint duration;
        };

        static Button Controller::* button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);
        void updateScreen();

        std::array<std::deque<Line>, 4> screen_buffer{};
        std::array<Line, 3> screen_contents{};
        std::array<uint32_t, 3> line_set_time{};
        uint8_t last_printed_line = 0;
        uint32_t last_print_time = 0;
        pros::Controller controller;
}; // namespace Gamepad
}