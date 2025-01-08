#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include "abstractScreen.hpp"
#include "pros/rtos.hpp"
#include "gamepad/screens/abstractScreen.hpp"

namespace gamepad {

/**
 * @brief a screen that sends alerts to the controller, duration of alerts can be customized
 *
 * @note priority: UINT32_MAX - 100
 */
class AlertScreen : public AbstractScreen {
    public:
        AlertScreen()
            : AbstractScreen(UINT32_MAX - 100) {}

        /**
         * @brief updates the alert loop
         *
         * @param delta_time the time since the last update
         */
        void update(uint32_t delta_time);

        /**
         * @brief return the next alert to print if there is space for it on the screen
         *
         * @param visible_lines a set that contains the line numbers of all lines that
         * are empty and available for printing
         *
         * @returns a the lines to be printed, any lines that are not available will be ignored
         */
        ScreenBuffer getScreen(std::set<uint8_t> visible_lines);

        /**
         * @brief add an alert to the alert queue, to be printed as soon as there is an available space
         *
         * @param line the line number to print the alert at (0-2)
         * @param strs the string to print on the controller, "\n" to go to the next line
         * lines that go over 2 will be cropped out
         * @param duration how long the alert should persist on the screen
         * @param rumble A string consisting of the characters '.', '-', and ' ', where dots are short rumbles,
         * dashes are long rumbles, and spaces are pauses. Maximum supported length is 8 characters.
         *
         * @return 0 if the alert was added successfully, UINT32_MAX if there was an error.
         */
        uint32_t addAlerts(uint8_t line, std::string strs, uint32_t duration, std::string rumble = "");
    private:
        struct AlertBuffer {
                ScreenBuffer screen;
                uint32_t duration;
        };

        std::deque<AlertBuffer> m_screen_buffer {};
        std::optional<AlertBuffer> m_screen_contents {};
        uint32_t m_line_set_time = 0;
        pros::Mutex m_mutex {};
};

} // namespace gamepad
