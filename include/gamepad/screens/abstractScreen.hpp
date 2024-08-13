#pragma once

#include <array>
#include <optional>
#include <set>
#include <string>
#include "pros/misc.h"

namespace Gamepad {

/**
 * @brief type for conveying a full screen with the first 3 being the lines
 * of text on the controller screen and the last being a rumble pattern 
 */
typedef std::array<std::optional<std::string>, 4> ScreenBuffer;

class AbstractScreen {
    public:
        AbstractScreen(uint priority): priority(priority) {}
        void update(double delta_time) {}
        virtual ScreenBuffer get_screen(std::set<uint8_t> visible_lines) = 0;
        virtual void handle_events(std::set<pros::controller_digital_e_t> button_events) = 0;
        const uint get_priority();
    private:
        const uint priority;
};

} // namespace Gamepad