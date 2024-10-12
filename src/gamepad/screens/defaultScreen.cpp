#include "gamepad/screens/defaultScreen.hpp"
#include "gamepad/screens/abstractScreen.hpp"
#include "gamepad/todo.hpp"
#include <algorithm>
#include <cstdio>
#include <mutex>
#include <optional>
#include <sstream>

namespace gamepad {

DefaultScreen::DefaultScreen()
    : AbstractScreen(1),
      currentBuffer({}) {}

ScreenBuffer DefaultScreen::get_screen(std::set<uint8_t> visible_lines) {
    ScreenBuffer output;
    
    printf("currentBuffer = {%s, %s, %s, %s}\n", currentBuffer.at(0).value_or("nullopt").c_str(),
           currentBuffer.at(1).value_or("nullopt").c_str(), currentBuffer.at(2).value_or("nullopt").c_str(),
           currentBuffer.at(3).value_or("nullopt").c_str());

    const std::lock_guard<pros::Mutex> guard(this->mut);
    for (auto i = visible_lines.begin(); i != visible_lines.end(); ++i) {
        output[*i] = std::move(this->currentBuffer[*i].value_or(""));
        this->currentBuffer[*i] = std::nullopt;
    }
    return output;
}

void DefaultScreen::print_line(uint8_t line, std::string str) {
    TODO("change handling for off screen lines")
    if (line > 2) std::exit(1);

    const std::lock_guard<pros::Mutex> guard(this->mut);

    if (str.find('\n') != std::string::npos) {
        TODO("warn instead of throw error if there are too many lines")
        if (std::ranges::count(str, '\n') > 2) std::exit(1);

        std::vector<std::string> strs(3);
        std::stringstream ss(str);

        for (int i = line; i < 3; i++) {
            if (!std::getline(ss, strs[i], '\n')) break;
        }

        for (uint8_t l = 0; l < 3; l++) {
            if (!strs[l].empty()) this->currentBuffer[l] = (strs[l]);
        }
        return;
    }

    this->currentBuffer[line] = std::move(str);
}

void DefaultScreen::rumble(std::string rumble_pattern) {
    TODO("change handling for too long rumble patterns")
    if (rumble_pattern.size() > 8) std::exit(1);

    std::lock_guard<pros::Mutex> guard(this->mut);
    this->currentBuffer[3] = std::move(rumble_pattern);
}

} // namespace gamepad