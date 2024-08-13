#pragma once

#include "gamepad/screens/abstractScreen.hpp"
#include "pros/rtos.hpp"

namespace Gamepad {

class DefaultScreen : AbstractScreen {
    public:
        DefaultScreen() : AbstractScreen(1) {}
        ScreenBuffer get_screen(std::set<uint8_t> visible_lines);
        void handle_events(std::set<pros::controller_digital_e_t> button_events) {}

        void print_line(uint8_t line, std::string str);
        void rumble(std::string rumble_pattern);

    private:
        ScreenBuffer currentBuffer;
        pros::Mutex mut;

};

}