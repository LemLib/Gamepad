#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include "pros/misc.h"

namespace gamepad {

/**
 * @brief type for conveying a full screen with the first 3 being the lines
 * of text on the controller screen and the last being a rumble pattern
 */
typedef std::array<std::optional<std::string>, 4> ScreenBuffer;

/**
 * @brief The abstract class for interacting with the controller screen
 *
 */
class AbstractScreen {
    public:
        AbstractScreen(uint32_t priority)
            : m_priority(priority) {}

        /**
         * @brief runs every time the controller's update function is called
         * use this if you need to update something regardless of if there is an
         * available slot in the screen
         *
         * @param delta_time the time since the last update in milliseconds
         */
        virtual void update(uint32_t delta_time) {}

        /**
         * @brief runs if there is an empty line that is available to print
         *
         * @param visible_lines a set that contains the line numbers of all lines that
         * are empty and available for printing
         *
         * @returns a the lines to be printed, any lines that are not available will be ignored
         */
        virtual ScreenBuffer getScreen(std::set<uint8_t> visible_lines) = 0;

        /**
         * @brief a function where button events are pushed, use this to handle button events.
         *
         * @param button_events a set of the button events that happened this update
         */
        virtual void handleEvents(std::set<pros::controller_digital_e_t> button_events) {}

        /**
         * @brief returns the priority of the screen
         *
         * @warning it is not recommended to override this function
         */
        uint32_t getPriority() { return m_priority; }
    protected:
        const uint32_t m_priority;
};

} // namespace gamepad
