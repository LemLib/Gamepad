#include "gamepad/screens/alertScreen.hpp"
#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cstdint>
#include <mutex>
#include <optional>
#include <sstream>

namespace gamepad {

ScreenBuffer AlertScreen::getScreen(std::set<uint8_t> visible_lines) {
    std::lock_guard<pros::Mutex> guard(m_mutex);
    if (m_screen_contents.has_value()) {
        m_screen_contents->screen.at(3) = std::nullopt;
        return m_screen_contents->screen;
    }
    if (m_screen_buffer.size() < 1) return ScreenBuffer();

    for (uint8_t i = 0; i < 4; i++) {
        if (!m_screen_buffer[0].screen[i].has_value()) continue;
        if (m_screen_buffer[0].screen[i].has_value() && !visible_lines.contains(i)) return ScreenBuffer();
    }
    m_screen_contents = std::move(m_screen_buffer[0]);
    m_screen_buffer.pop_front();
    m_line_set_time = pros::millis();
    return m_screen_contents->screen;
}

void AlertScreen::update(uint32_t delta_time) {
    std::lock_guard<pros::Mutex> guard(m_mutex);
    if (pros::millis() - m_line_set_time >= m_screen_contents->duration) m_screen_contents = std::nullopt;
}

void AlertScreen::addAlerts(uint8_t line, std::string str, uint32_t duration, std::string rumble) {
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

    std::lock_guard<pros::Mutex> guard(m_mutex);
    m_screen_buffer.push_back({buffer, duration});
}

} // namespace gamepad
