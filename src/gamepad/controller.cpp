#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>

namespace Gamepad {

uint32_t Button::onPress(std::function<void(void)> func) {
    return this->onPressEvent.add_listener(std::move(func));
}

uint32_t Button::onLongPress(std::function<void(void)> func) {
    return this->onLongPressEvent.add_listener(std::move(func));
}

uint32_t Button::onRelease(std::function<void(void)> func) {
    return this->onReleaseEvent.add_listener(std::move(func));
}

void Button::update(const bool is_held) {
    static uint32_t last_update_time = pros::millis();

    this->rising_edge = !this->is_pressed && is_held;
    this->falling_edge = this->is_pressed && !is_held;
    this->is_pressed = is_held;
    if (is_held) {
        this->time_held += pros::millis() - last_update_time;
    } else {
        this->time_released += pros::millis() - last_update_time;
    }
    if (this->rising_edge) {
        this->time_held = 0;
    } 
    if (this->falling_edge) {
        this->time_released = 0;
    }

    if (this->rising_edge) {
        onPressEvent.fire();
    } else if (this->falling_edge) {
        onReleaseEvent.fire();
    }
    TODO("implement longPress");
    last_update_time = pros::millis();
}

void Controller::updateButton(pros::controller_digital_e_t button_id) {
    Button Controller::* button = Controller::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Controller::updateScreen() {
    if (pros::millis() - this->last_print_time < 50)
        return;

    for (int i = 1; i <= 4; i++) {
        int line = (this->last_printed_line + i) % 4;

        // not part of the screen so rumble
        if (line == 3) {
            this->controller.rumble(this->next_print[line].c_str());
            this->next_print[line] = "";
            this->last_printed_line = line;
            this->last_print_time = pros::millis();
            return;
        }

        // no alerts so print from string
        if (this->screen_buffer[line].size() == 0 && next_print[line] != "") {
            this->controller.set_text(line, 0, this->next_print[line] + std::string(40, ' '));
            this->next_print[line] = "";
            this->last_printed_line = line;
            this->last_print_time = pros::millis();
            return;
        }

        if (screen_buffer[line].size() == 0) return;

        // else print to screen
        if (pros::millis() - this->line_set_time[line] < this->screen_contents[line].duration)
            continue;

        this->controller.set_text(line, 0, this->screen_buffer[line][0].text + std::string(40, ' '));  
        this->screen_contents[line] = this->screen_buffer[line][0];
        this->screen_buffer[line].pop_front();
        
        this->last_printed_line = line;
        this->line_set_time[line] = pros::millis();
        this->last_print_time = pros::millis();
        return;
    }
    
}

void Controller::update() {
    for(int i = DIGITAL_L1; i != DIGITAL_A; ++i) {
        this->updateButton(static_cast<pros::controller_digital_e_t>(i));
    }

    this->LeftX = this->controller.get_analog(ANALOG_LEFT_X);
    this->LeftY = this->controller.get_analog(ANALOG_LEFT_Y);
    this->RightX = this->controller.get_analog(ANALOG_RIGHT_X);
    this->RightY = this->controller.get_analog(ANALOG_RIGHT_Y);

    this->updateScreen();
}

uint Controller::getTotalDuration(uint8_t line) {
    uint total = 0; 
    for (Line msg : this->screen_buffer[line])
        total += msg.duration;
    return total;
}

void Controller::add_alerts(uint8_t line, std::string str, uint32_t duration) {
    TODO("change handling for off screen lines")
    if (line > 2) std::exit(1);

    this->screen_buffer[line].push_back({ .text = std::move(str), .duration = duration });
}

void Controller::add_alerts(std::vector<uint8_t> lines, std::vector<std::string> strs, uint32_t duration) {
    assert(lines.size() == strs.size());

    // get nex available time slot for all lines
    uint minSpot = -1; // max uint value
    for (uint8_t line : lines) {
        TODO("change handling for off screen lines")
        if (line > 2) std::exit(1);

        if (getTotalDuration(line) < minSpot)
            minSpot = getTotalDuration(line);
    }

    for (int i = 0; i < lines.size(); i++) {
        if (getTotalDuration(lines[i]) < minSpot)
            this->screen_buffer[lines[i]].push_back({ .text = "", .duration = (minSpot - getTotalDuration(lines[i])) });
        this->screen_buffer[lines[i]].push_back({ .text = std::move(strs[i]), .duration = duration });
    }
}

void Controller::print_line(uint8_t line, std::string str) {
    TODO("change handling for off screen lines")
    if (line > 2) std::exit(1);

    this->next_print[line] = std::move(str);
}

void Controller::rumble(std::string rumble_pattern) {
    TODO("change handling for too long rumble patterns")
    if (rumble_pattern.size() > 8) std::exit(1);

    this->next_print[3] = std::move(rumble_pattern);
}

const Button& Controller::operator[](pros::controller_digital_e_t button) {
    return this->*Controller::button_to_ptr(button);
}

float Controller::operator[](pros::controller_analog_e_t axis) {
    switch (axis) {
        case ANALOG_LEFT_X: return this->LeftX;
        case ANALOG_LEFT_Y: return this->LeftY;
        case ANALOG_RIGHT_X: return this->RightX;
        case ANALOG_RIGHT_Y: return this->RightY;
        TODO("change handling for default")
        default: std::exit(1);
    }
}

Button Controller::* Controller::button_to_ptr(pros::controller_digital_e_t button) {
    switch (button) {
        case DIGITAL_L1: return &Controller::L1;
        case DIGITAL_L2: return &Controller::L2;
        case DIGITAL_R1: return &Controller::R1;
        case DIGITAL_R2: return &Controller::R2;
        case DIGITAL_UP: return &Controller::Up;
        case DIGITAL_DOWN: return &Controller::Down;
        case DIGITAL_LEFT: return &Controller::Left;
        case DIGITAL_RIGHT: return &Controller::Right;
        case DIGITAL_X: return &Controller::X;
        case DIGITAL_B: return &Controller::B;
        case DIGITAL_Y: return &Controller::Y;
        case DIGITAL_A: return &Controller::A;
        TODO("change handling for default")
        default: std::exit(1);
    }
}
}