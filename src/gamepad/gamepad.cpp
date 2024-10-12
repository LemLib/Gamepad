#include "gamepad/gamepad.hpp"
#include "gamepad/todo.hpp"
#include "pros/misc.h"
#include "pros/rtos.hpp"
#include <sys/types.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <atomic>

namespace gamepad {
void Gamepad::updateButton(pros::controller_digital_e_t button_id) {
    Button Gamepad::* button = Gamepad::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Gamepad::updateScreens() {
    // Lock Mutexes for Thread Safety
    std::lock_guard<pros::Mutex> guard_scheduling(this->mut);

    // Update all screens and note deltatime
    for (int i = 0; i < this->screens.size(); i++) this->screens[i]->update(pros::millis() - this->last_update_time);
    last_update_time = pros::millis();

    // Check if enough time has passed for the Gamepad to poll for updates
    if (pros::millis() - this->last_print_time < 50) return;

    for (int i = 0; i < this->screens.size(); i++) {
        // get all lines that arent being used by a higher priority screen
        std::set<uint8_t> visible_lines;
        for (uint8_t j = 0; j < 4; j++)
            if (!this->nextBuffer[j].has_value()) visible_lines.emplace(j);

        // get the buffer of the next lower priority screen and set it to be printed
        ScreenBuffer buffer = this->screens[i]->get_screen(visible_lines);
        for (uint8_t j = 0; j < 4; j++)
            if (buffer[j].has_value() && !buffer[j]->empty() && !nextBuffer[j].has_value())
                nextBuffer[j] = std::move(buffer[j]);
    }

    for (int i = 0; i < 4; i++) {
        // start from the line thats after the line thats been set so we dont get stuck setting the first line
        int line = (this->last_printed_line + i) % 4;
        printf("Line = %i\n", line);

        printf("nextBuffer = {%s, %s, %s, %s}\n", nextBuffer.at(0).value_or("nullopt").c_str(),
               nextBuffer.at(1).value_or("nullopt").c_str(), nextBuffer.at(2).value_or("nullopt").c_str(),
               nextBuffer.at(3).value_or("nullopt").c_str());

        printf("currentScreen = {%s, %s, %s, %s}\n", currentScreen.at(0).value_or("nullopt").c_str(),
               currentScreen.at(1).value_or("nullopt").c_str(), currentScreen.at(2).value_or("nullopt").c_str(),
               currentScreen.at(3).value_or("nullopt").c_str());

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

    // just to make the cycle more regular
    this->last_printed_line = 0;
}

void Gamepad::update() {
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i <= pros::E_CONTROLLER_DIGITAL_A; ++i) {
        this->updateButton(static_cast<pros::controller_digital_e_t>(i));
    }

    this->m_LeftX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    this->m_LeftY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    this->m_RightX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    this->m_RightY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

    this->updateScreens();
}

void Gamepad::add_screen(std::shared_ptr<AbstractScreen> screen) {
    uint last = UINT32_MAX;
    uint pos = 0;
    for (pos = 0; pos < this->screens.size(); pos++) {
        if (this->screens[pos]->get_priority() < screen->get_priority() && last >= screen->get_priority()) break;
        last = this->screens[pos]->get_priority();
    }
    this->screens.emplace(this->screens.begin() + pos, screen);
}

void Gamepad::print_line(uint8_t line, std::string str) { this->defaultScreen->print_line(line, str); }

void Gamepad::clear() { this->defaultScreen->print_line(0, " \n \n "); }

void Gamepad::clear(uint8_t line) { this->defaultScreen->print_line(line, " "); }

void Gamepad::rumble(std::string rumble_pattern) { this->defaultScreen->rumble(rumble_pattern); }

const Button& Gamepad::operator[](pros::controller_digital_e_t button) { return this->*Gamepad::button_to_ptr(button); }

float Gamepad::operator[](pros::controller_analog_e_t axis) {
    switch (axis) {
        case pros::E_CONTROLLER_ANALOG_LEFT_X: return this->LeftX;
        case pros::E_CONTROLLER_ANALOG_LEFT_Y: return this->LeftY;
        case pros::E_CONTROLLER_ANALOG_RIGHT_X: return this->RightX;
        case pros::E_CONTROLLER_ANALOG_RIGHT_Y: return this->RightY;
        default:
            TODO("add error logging")
            errno = EINVAL;
            return 0;
    }
}

std::string Gamepad::unique_name() {
    static std::atomic<uint32_t> i = 0;
    return std::to_string(i++) + "_internal";
}

Button Gamepad::* Gamepad::button_to_ptr(pros::controller_digital_e_t button) {
    switch (button) {
        case pros::E_CONTROLLER_DIGITAL_L1: return &Gamepad::m_L1;
        case pros::E_CONTROLLER_DIGITAL_L2: return &Gamepad::m_L2;
        case pros::E_CONTROLLER_DIGITAL_R1: return &Gamepad::m_R1;
        case pros::E_CONTROLLER_DIGITAL_R2: return &Gamepad::m_R2;
        case pros::E_CONTROLLER_DIGITAL_UP: return &Gamepad::m_Up;
        case pros::E_CONTROLLER_DIGITAL_DOWN: return &Gamepad::m_Down;
        case pros::E_CONTROLLER_DIGITAL_LEFT: return &Gamepad::m_Left;
        case pros::E_CONTROLLER_DIGITAL_RIGHT: return &Gamepad::m_Right;
        case pros::E_CONTROLLER_DIGITAL_X: return &Gamepad::m_X;
        case pros::E_CONTROLLER_DIGITAL_B: return &Gamepad::m_B;
        case pros::E_CONTROLLER_DIGITAL_Y: return &Gamepad::m_Y;
        case pros::E_CONTROLLER_DIGITAL_A: return &Gamepad::m_A;
        default:
            TODO("add error logging")
            errno = EINVAL;
            return &Gamepad::Fake;
    }
}
} // namespace gamepad
