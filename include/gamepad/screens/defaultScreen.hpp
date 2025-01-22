#pragma once

#include "gamepad/screens/abstractScreen.hpp"
#include "pros/rtos.hpp"

namespace gamepad {

/**
 * @brief A basic screen that allows basic prints, similar to pros controller api
 *
 * @note The gamepad class has wrappers around this class
 * @note priority: 1
 */
class DefaultScreen : public AbstractScreen {
    public:
        DefaultScreen()
            : AbstractScreen(1) {}

        /**
         * @brief returns any lines that have space to print on the controller
         *
         * @param visible_lines a set that contains the line numbers of all lines that
         * are empty and available for printing
         *
         * @returns a the lines to be printed, any lines that are not available will be ignored
         */
        ScreenBuffer getScreen(std::set<uint8_t> visible_lines);

        /**
         * @brief print a line to the console like pros
         *
         * @param line the line number to print the string on (0-2)
         * @param str the string to print onto the controller (\n to go to the next line)
         *
         * This function uses the following value(s) of errno when an error state is reached:
         *
         * EINVAL: The line number is not in the interval [0, 2]
         *
         * @return 0 if the alert was added successfully
         * @return INT32_MAX if there was an error, setting errno
         */
        int32_t printLine(uint8_t line, std::string str);

        /**
         * makes the controller rumble like pros
         *
         * @param rumble_pattern A string consisting of the characters '.', '-', and ' ', where dots are short rumbles,
         * dashes are long rumbles, and spaces are pauses. Maximum supported length is 8 characters.
         */
        int32_t rumble(std::string rumble_pattern);
    private:
        ScreenBuffer m_current_buffer {};
        pros::Mutex m_mutex {};
};

} // namespace gamepad
