#include "gamepad/screens/alertScreen.hpp"
#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cstdint>
#include <mutex>
#include <optional>
#include <sstream>

namespace gamepad {

ScreenBuffer AlertScreen::get_screen(std::set<uint8_t> visible_lines) {
    std::lock_guard<pros::Mutex> guard(this->mut);
    if (this->screen_contents.has_value()) {
        this->screen_contents->screen.at(3) = std::nullopt;
        return this->screen_contents->screen;
    }
    if (this->screen_buffer.size() < 1) return ScreenBuffer();

    for (uint8_t i = 0; i < 4; i++) {
        if (!this->screen_buffer[0].screen[i].has_value()) continue;
        if (this->screen_buffer[0].screen[i].has_value() && !visible_lines.contains(i)) return ScreenBuffer();
    }
    this->screen_contents = std::move(this->screen_buffer[0]);
    this->screen_buffer.pop_front();
    this->line_set_time = pros::millis();
    return this->screen_contents->screen;
}

void AlertScreen::update(uint32_t delta_time) {
    std::lock_guard<pros::Mutex> guard(this->mut);
    if (pros::millis() - this->line_set_time >= this->screen_contents->duration) this->screen_contents = std::nullopt;
}

void AlertScreen::add_alerts(uint8_t line, std::string str, uint32_t duration, std::string rumble) {
    TODO("change handling for off screen lines")
    if (line > 2) std::exit(1);

    TODO("warn instead of throw error if there are too many lines")
    if (std::ranges::count(str, '\n') > 2) std::exit(1);

    std::vector<std::string> strs(3, "");
    std::stringstream ss(str);

    // split string by newlines but only take the first 3 lines
    for (int i = line; i < 3; i++) {
        if (!std::getline(ss, strs[i], '\n')) break;
    }

    ScreenBuffer buffer;

    if (strs[0] != "") buffer[0] = std::move(strs[0]);
    if (strs[1] != "") buffer[1] = std::move(strs[1]);
    if (strs[2] != "") buffer[2] = std::move(strs[2]);
    if (rumble != "") buffer[3] = std::move(rumble);

    std::lock_guard<pros::Mutex> guard(this->mut);
    this->screen_buffer.push_back({buffer, duration});
}

} // namespace gamepad
