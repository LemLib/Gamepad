#pragma once

#include "pros/misc.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <sys/types.h>
#ifndef PROS_USE_SIMPLE_NAMES
#define PROS_USE_SIMPLE_NAMES
#endif
#include "screens/abstractScreen.hpp"
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
        /**
         * Updates the state of the gamepad (all joysticks and buttons), and also runs
         * any registered handlers.
         * @note This function should be called at the beginning of every loop iteration.
         * @note Create a separate instance for each task.
         */
        void update();

        void add_screen(std::shared_ptr<AbstractScreen> screen);
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
        Button L1 {}, L2 {}, R1 {}, R2 {}, Up {}, Down {}, Left {}, Right {}, X {}, B {}, Y {}, A {};
        float LeftX = 0, LeftY = 0, RightX = 0, RightY = 0;
        /// The master controller, same as @ref Gamepad::master
        static Controller master;
        /// The partner controller, same as @ref Gamepad::partner
        static Controller partner;
    private:
        explicit Controller(pros::controller_id_e_t id)
            : controller(id) {}

        static Button Controller::*button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);
        
        void updateScreens();

        std::vector<std::shared_ptr<AbstractScreen>> screens;
        ScreenBuffer currentScreen;
        ScreenBuffer nextBuffer;
        pros::Controller controller;

        uint8_t last_printed_line = 0;
        uint last_print_time = 0;
        uint last_update_time = 0;
        pros::Mutex mut;
};

inline Controller Controller::master {pros::E_CONTROLLER_MASTER};
inline Controller Controller::partner {pros::E_CONTROLLER_PARTNER};
/// The master controller
inline Controller& master = Controller::master;
/// The partner controller
inline Controller& partner = Controller::partner;
} // namespace Gamepad
