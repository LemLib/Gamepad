#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"
#include "pros/misc.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace Gamepad {
uint32_t Button::onPress(std::function<void(void)> func) { return this->onPressEvent.add_listener(std::move(func)); }

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
    if (this->rising_edge) { this->time_held = 0; }
    if (this->falling_edge) { this->time_released = 0; }

    if (this->rising_edge) {
        onPressEvent.fire();
    } else if (this->falling_edge) {
        onReleaseEvent.fire();
    }
    TODO("implement longPress");
    last_update_time = pros::millis();
}

void Controller::updateButton(pros::controller_digital_e_t button_id) {
    Button Controller::*button = Controller::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Controller::updateScreens() {
    // Lock Mutexes for Thread Safety
    std::lock_guard<pros::Mutex> guard_scheduling(this->mut);

    // Update all screens and note deltatime
    for (int i = 0; i < this->screens.size(); i++)
        this->screens[i]->update(pros::millis() - this->last_update_time);
    last_update_time = pros::millis();

    // Check if enough time has passed for the controller to poll for updates
    if (pros::millis() - this->last_print_time < 50)
        return;

    for (int i = 0; i < this->screens.size(); i++) {
        // get all lines that arent being used by a higher priority screen
        std::set<uint8_t> visible_lines;
        for (uint8_t j = 0; j < 4; j++)
            if (!this->nextBuffer[j].has_value())
                visible_lines.emplace(j);
        
        // get the buffer of the next lower priority screen and set it to be printed
        ScreenBuffer buffer = this->screens[i]->get_screen(visible_lines);
        for (uint8_t j = 0; j < 4; j++)
            if (buffer[j].has_value() && !nextBuffer[j].has_value())
                nextBuffer[j] = std::move(buffer[j]);
    }

    for (int i = 1; i <= 4; i++) {
        // start from the line thats after the line thats been set so we dont get stuck setting the first line
        int line = (this->last_printed_line + i) % 4;

        // text on screen is the same as last frame's text so no use updating
        if (this->currentScreen[line] == this->nextBuffer[line] && line != 3) {
            this->nextBuffer[line] = std::nullopt;
            continue;
        }

        // print to screen or rumble
        if (line == 3) this->controller.rumble(this->nextBuffer[line].value_or("").c_str());
        else this->controller.set_text(line, 0, this->nextBuffer[line].value_or("") + std::string(40, ' '));
        this->currentScreen[line] = std::move(this->nextBuffer[line]);
        this->nextBuffer[line] = std::nullopt;
        this->last_printed_line = line;
        this->last_print_time = pros::millis();
    }
}

void Controller::update() {
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i != pros::E_CONTROLLER_DIGITAL_A; ++i) {
        this->updateButton(static_cast<pros::controller_digital_e_t>(i));
    }

    this->LeftX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    this->LeftY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    this->RightX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    this->RightY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
    
    this->updateScreens();
}

void Controller::add_screen(std::shared_ptr<AbstractScreen> screen) {
    uint last = UINT32_MAX; uint pos = 0;
    for (pos = 0; pos < this->screens.size(); pos++) {
        if (this->screens[pos]->get_priority() < screen->get_priority() && last >= screen->get_priority())
            break;
        last = this->screens[pos]->get_priority();
    }
    this->screens.emplace(this->screens.begin() + pos, screen);
}


const Button& Controller::operator[](pros::controller_digital_e_t button) {
    return this->*Controller::button_to_ptr(button);
}

float Controller::operator[](pros::controller_analog_e_t axis) {
    switch (axis) {
        case pros::E_CONTROLLER_ANALOG_LEFT_X: return this->LeftX;
        case pros::E_CONTROLLER_ANALOG_LEFT_Y: return this->LeftY;
        case pros::E_CONTROLLER_ANALOG_RIGHT_X: return this->RightX;
        case pros::E_CONTROLLER_ANALOG_RIGHT_Y: return this->RightY; TODO("change handling for default")
        default: std::exit(1);
    }
}

Button Controller::*Controller::button_to_ptr(pros::controller_digital_e_t button) {
    switch (button) {
        case pros::E_CONTROLLER_DIGITAL_L1: return &Controller::L1;
        case pros::E_CONTROLLER_DIGITAL_L2: return &Controller::L2;
        case pros::E_CONTROLLER_DIGITAL_R1: return &Controller::R1;
        case pros::E_CONTROLLER_DIGITAL_R2: return &Controller::R2;
        case pros::E_CONTROLLER_DIGITAL_UP: return &Controller::Up;
        case pros::E_CONTROLLER_DIGITAL_DOWN: return &Controller::Down;
        case pros::E_CONTROLLER_DIGITAL_LEFT: return &Controller::Left;
        case pros::E_CONTROLLER_DIGITAL_RIGHT: return &Controller::Right;
        case pros::E_CONTROLLER_DIGITAL_X: return &Controller::X;
        case pros::E_CONTROLLER_DIGITAL_B: return &Controller::B;
        case pros::E_CONTROLLER_DIGITAL_Y: return &Controller::Y;
        case pros::E_CONTROLLER_DIGITAL_A: return &Controller::A; TODO("change handling for default")
        default: std::exit(1);
    }
}
} // namespace Gamepad