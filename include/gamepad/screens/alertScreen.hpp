#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>
#include "pros/rtos.hpp"
#include "gamepad/screens/abstractScreen.hpp"

namespace Gamepad {

class AlertScreen : AbstractScreen {
    public:
        AlertScreen() : AbstractScreen(UINT32_MAX - 100) {}
        void update(double delta_time);
        ScreenBuffer get_screen(std::set<uint8_t> visible_lines);
        void handle_events(std::set<pros::controller_digital_e_t> button_events);

        void add_alerts(uint8_t line, std::string strs, uint32_t duration, std::string rumble = "");

    private:
        struct Line {
            std::string text;
            uint duration;
        };

        uint getTotalDuration(uint8_t line);

        std::array<std::deque<Line>, 4> screen_buffer{};
        std::array<Line, 4> screen_contents{};
        std::array<uint32_t, 4> line_set_time{};
        pros::Mutex mut;
};

}