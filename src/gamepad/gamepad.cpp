#include "gamepad/gamepad.hpp"
#include "gamepad/todo.hpp"
#include "pros/misc.h"
#include "pros/rtos.hpp"
#include "screens/abstractScreen.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <atomic>

namespace gamepad {
Gamepad::Gamepad(pros::controller_id_e_t id)
    : m_controller(id) {
    this->addScreen(m_default_screen);
}

void Gamepad::updateButton(pros::controller_digital_e_t button_id) {
    Button Gamepad::* button = Gamepad::buttonToPtr(button_id);
    bool is_held = m_controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Gamepad::updateScreens() {
    // Lock Mutexes for Thread Safety
    std::lock_guard<pros::Mutex> guard_scheduling(m_mutex);

    // Disable screen updates if the controller is disconnected
    if (!m_controller.is_connected()) {
        if (m_screen_cleared) {
            m_next_buffer = std::move(m_current_screen);
            m_current_screen = {};
            m_screen_cleared = false;
        }
        return;
    }

    // Clear current screen and reset last update time on reconnect
    if (m_controller.is_connected() && !m_screen_cleared) {
        m_current_screen = {};
        m_last_update_time = pros::millis();
    }

    // Get new button presses
    std::set<pros::controller_digital_e_t> buttonUpdates;
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i <= pros::E_CONTROLLER_DIGITAL_A; ++i) {
        if ((this->*this->buttonToPtr(static_cast<pros::controller_digital_e_t>(i))).rising_edge) {
            buttonUpdates.emplace(static_cast<pros::controller_digital_e_t>(i));
        }
    }

    // Update all screens, and send new button presses, also note deltatime
    for (std::shared_ptr<AbstractScreen> screen : m_screens) {
        screen->update(pros::millis() - m_last_update_time);
        screen->handleEvents(buttonUpdates);
    }
    m_last_update_time = pros::millis();

    // Check if enough time has passed for the Gamepad to poll for updates
    if (pros::millis() - m_last_print_time <= 50) return;

    for (std::shared_ptr<AbstractScreen> screen : m_screens) {
        // get all lines that aren't being used by a higher priority screen
        std::set<uint8_t> visible_lines;
        for (uint8_t j = 0; j < 4; j++)
            if (!m_next_buffer[j].has_value()) visible_lines.emplace(j);

        // get the buffer of the next lower priority screen and set it to be printed
        ScreenBuffer buffer = screen->getScreen(visible_lines);
        for (uint8_t j = 0; j < 4; j++)
            if (buffer[j].has_value() && !buffer[j]->empty() && !m_next_buffer[j].has_value())
                m_next_buffer[j] = std::move(buffer[j]);
    }

    for (int i = 0; i < 4; i++) {
        // start from the line thats after the line thats been set so we dont get stuck setting the first line
        int line = (m_last_printed_line + i) % 4;

        // theres nothing on this line so we can skip it
        if (!m_next_buffer[line].has_value()) continue;

        if (!m_screen_cleared && line != 3) {
            m_controller.clear();
            m_screen_cleared = true;
            m_current_screen = {};
            m_last_print_time = pros::millis();
            return;
        }

        // text on screen is the same as last frame's text so no use updating
        if (m_current_screen[line] == m_next_buffer[line] && line != 3) {
            m_next_buffer[line] = std::nullopt;
            continue;
        }

        // print to screen or rumble
        if (line == 3) m_controller.rumble(m_next_buffer[line].value_or("").c_str());
        else m_controller.set_text(line, 0, m_next_buffer[line].value_or("") + std::string(40, ' '));
        if (line != 3) m_current_screen[line] = std::move(m_next_buffer[line]);
        m_next_buffer[line] = std::nullopt;
        m_last_printed_line = line;
        m_last_print_time = pros::millis();
        return;
    }
}

void Gamepad::update() {
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i <= pros::E_CONTROLLER_DIGITAL_A; ++i) {
        this->updateButton(static_cast<pros::controller_digital_e_t>(i));
    }

    m_LeftX = m_controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X) / 127.0;
    m_LeftY = m_controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) / 127.0;
    m_RightX = m_controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X) / 127.0;
    m_RightY = m_controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) / 127.0;

    this->updateScreens();
}

void Gamepad::addScreen(std::shared_ptr<AbstractScreen> screen) {
    uint32_t last = UINT32_MAX;
    uint32_t pos = 0;
    for (pos = 0; pos < m_screens.size(); pos++) {
        if (m_screens[pos]->getPriority() < screen->getPriority() && last >= screen->getPriority()) break;
        last = m_screens[pos]->getPriority();
    }
    m_screens.emplace(m_screens.begin() + pos, screen);
}

int32_t Gamepad::printLine(uint8_t line, std::string str) { return m_default_screen->printLine(line, str); }

void Gamepad::clear() { m_default_screen->printLine(0, " \n \n "); }

int32_t Gamepad::clear(uint8_t line) { return m_default_screen->printLine(line, " "); }

void Gamepad::rumble(std::string rumble_pattern) { m_default_screen->rumble(rumble_pattern); }

const Button& Gamepad::operator[](pros::controller_digital_e_t button) { return this->*Gamepad::buttonToPtr(button); }

float Gamepad::operator[](pros::controller_analog_e_t axis) {
    switch (axis) {
        case pros::E_CONTROLLER_ANALOG_LEFT_X: return m_LeftX;
        case pros::E_CONTROLLER_ANALOG_LEFT_Y: return m_LeftY;
        case pros::E_CONTROLLER_ANALOG_RIGHT_X: return m_RightX;
        case pros::E_CONTROLLER_ANALOG_RIGHT_Y: return m_RightY;
        default: TODO("add error logging") return 0;
    }
}

const Button& Gamepad::buttonL1() { return m_L1; }

const Button& Gamepad::buttonL2() { return m_L2; }

const Button& Gamepad::buttonR1() { return m_R1; }

const Button& Gamepad::buttonR2() { return m_R2; }

const Button& Gamepad::buttonUp() { return m_Up; }

const Button& Gamepad::buttonDown() { return m_Down; }

const Button& Gamepad::buttonLeft() { return m_Left; }

const Button& Gamepad::buttonRight() { return m_Right; }

const Button& Gamepad::buttonX() { return m_X; }

const Button& Gamepad::buttonB() { return m_B; }

const Button& Gamepad::buttonY() { return m_Y; }

const Button& Gamepad::buttonA() { return m_A; }

float Gamepad::axisLeftX(bool use_curve) {
    if (use_curve && m_left_transformation) return m_left_transformation->get_value({m_LeftX, m_LeftY}).first;
    else return m_LeftX;
}

float Gamepad::axisLeftY(bool use_curve) {
    if (use_curve && m_left_transformation) return m_left_transformation->get_value({m_LeftX, m_LeftY}).second;
    else return m_LeftY;
}

float Gamepad::axisRightX(bool use_curve) {
    if (use_curve && m_right_transformation) return m_right_transformation->get_value({m_RightX, m_RightY}).first;
    else return m_RightX;
}

float Gamepad::axisRightY(bool use_curve) {
    if (use_curve && m_right_transformation) return m_right_transformation->get_value({m_RightX, m_RightY}).second;
    else return m_RightY;
}

void Gamepad::set_left_transform(Transformation left_transformation) {
    m_left_transformation = std::move(left_transformation);
}

void Gamepad::set_right_transform(Transformation right_transformation) {
    m_right_transformation = std::move(right_transformation);
}

std::string Gamepad::uniqueName() {
    static std::atomic<uint32_t> i = 0;
    return std::to_string(i++) + "_internal";
}

Button Gamepad::* Gamepad::buttonToPtr(pros::controller_digital_e_t button) {
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
        default: TODO("add error logging") return &Gamepad::Fake;
    }
}
} // namespace gamepad
