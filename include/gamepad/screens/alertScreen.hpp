#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include "abstractScreen.hpp"
#include "pros/rtos.hpp"
#include "gamepad/screens/abstractScreen.hpp"

namespace Gamepad {

class AlertScreen : public AbstractScreen {
    public:
        AlertScreen() : AbstractScreen(UINT32_MAX - 100) {}
        void update(uint delta_time);
        ScreenBuffer get_screen(std::set<uint8_t> visible_lines);
        void handle_events(std::set<pros::controller_digital_e_t> button_events) {}

        void add_alerts(uint8_t line, std::string strs, uint32_t duration, std::string rumble = "");

    private:
        struct AlertBuffer {
            ScreenBuffer screen;
            uint duration;
        };

        std::deque<AlertBuffer> screen_buffer{};
        std::optional<AlertBuffer> screen_contents;
        uint line_set_time;
        pros::Mutex mut;
};

}
