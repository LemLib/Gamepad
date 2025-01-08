#include "gamepad/screens/defaultScreen.hpp"
#include "gamepad/screens/abstractScreen.hpp"
#include "gamepad/todo.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <optional>
#include <sstream>

namespace gamepad {

ScreenBuffer DefaultScreen::getScreen(std::set<uint8_t> visible_lines) {
    ScreenBuffer output;
    const std::lock_guard<pros::Mutex> guard(m_mutex);

    for (auto i = visible_lines.begin(); i != visible_lines.end(); ++i) {
        output[*i] = std::move(m_current_buffer[*i]);
        m_current_buffer[*i] = std::nullopt;
    }
    return output;
}

uint32_t DefaultScreen::printLine(uint8_t line, std::string str) {
    if (line > 2) {
        TODO("add error logging")
        errno = EINVAL;
        return UINT32_MAX;
    }

    const std::lock_guard<pros::Mutex> guard(m_mutex);

    if (str.find('\n') != std::string::npos) {
        if (std::ranges::count(str, '\n') > 2) { TODO("add warn logging for too many lines") }

        std::vector<std::string> strs(3);
        std::stringstream ss(str);

        for (int i = line; i < 3; i++) {
            if (!std::getline(ss, strs[i], '\n')) break;
        }

        for (uint8_t l = 0; l < 3; l++) {
            if (!strs[l].empty()) m_current_buffer[l] = (strs[l]);
        }
        return 0;
    }

    m_current_buffer[line] = std::move(str);
    return 0;
}

void DefaultScreen::rumble(std::string rumble_pattern) {
    if (rumble_pattern.size() > 8) {
        errno = EINVAL;
        TODO("add warn logging")
        rumble_pattern.resize(8);
    }

    std::lock_guard<pros::Mutex> guard(m_mutex);
    m_current_buffer[3] = std::move(rumble_pattern);
}

} // namespace gamepad