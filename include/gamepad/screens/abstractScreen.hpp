#pragma once

#include <array>
#include <optional>
#include <set>
#include <string>
#include <sys/types.h>
#include "pros/misc.h"

namespace gamepad {

/**
 * @brief type for conveying a full screen with the first 3 being the lines
 * of text on the controller screen and the last being a rumble pattern
 */
typedef std::array<std::optional<std::string>, 4> ScreenBuffer;

class AbstractScreen {
    public:
        AbstractScreen(uint priority)
            : priority(priority) {}

        virtual void update(uint delta_time) {}

        virtual ScreenBuffer get_screen(std::set<uint8_t> visible_lines) = 0;

        virtual void handle_events(std::set<pros::controller_digital_e_t> button_events) {}

        uint get_priority() { return this->priority; }
    protected:
        const uint priority;
};

} // namespace gamepad