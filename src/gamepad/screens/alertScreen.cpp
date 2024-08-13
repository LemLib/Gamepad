#include "gamepad/screens/alertScreen.hpp"
#include <algorithm>
#include <cstdint>
#include <mutex>
#include <sstream>
#include "gamepad/todo.hpp"

namespace Gamepad {

ScreenBuffer AlertScreen::get_screen(std::set<uint8_t> visible_lines) {
    ScreenBuffer output;
    
}


uint AlertScreen::getTotalDuration(uint8_t line) {
    uint total = 0; 
    for (Line msg : this->screen_buffer[line])
        total += msg.duration;
    return total;
}

void AlertScreen::add_alerts(uint8_t line, std::string str, uint32_t duration, std::string rumble) {
    TODO("change handling for off screen lines")
    if (line > 2) std::exit(1);

    if (str.find('\n') != std::string::npos) {
        TODO("warn instead of throw error if there are too many lines")
        if (std::ranges::count(str, '\n') > 2) std::exit(1);

        std::vector<std::string> strs(3);
        std::stringstream ss(str);

        // split string by newlines but only take the first 3 lines
        for (int i = line; i < 3; i++) {
            if (!std::getline(ss, strs[i], '\n')) break;
        }

        std::lock_guard<pros::Mutex> guard(this->mut);

        // get next available time slot for all lines
        uint minSpot = UINT32_MAX;
        for (uint8_t line = 0; line < 4; line++) minSpot = std::min(minSpot, getTotalDuration(line));

        // Schedule alerts
        for (int i = 0; i < 4; i++) {
            // add delay until theres a spot for all lines together
            if (getTotalDuration(i) < minSpot)
                this->screen_buffer[i].push_back({ .text = "", .duration = (minSpot - getTotalDuration(i)) });

            if (i == 3) this->screen_buffer[i].push_back({.text = std::move(rumble), .duration = duration});
            else this->screen_buffer[i].push_back({.text = std::move(strs[i]), .duration = 0});
        }
        return;
    }

    std::lock_guard<pros::Mutex> guard(this->mut);
    this->screen_buffer[line].push_back({ .text = std::move(str), .duration = duration });
}

}