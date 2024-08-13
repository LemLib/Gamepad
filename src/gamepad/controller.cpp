#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

void Controller::updateScreens() {
    // Lock Mutexes for Thread Safety
    std::lock_guard<pros::Mutex> guard_scheduling(this->mut);

    for (int i = 0; i < this->screens.size(); i++) {
        this->screens[i].update(pros::millis() - this->last_update_time);
    }

    // Check if enough time has passed for the controller to poll for updates
    if (pros::millis() - this->last_print_time < 50)
        return;

    for (int i = 1; i <= 4; i++) {
        // start from the line thats after the line thats been set so we dont get stuck setting the first line
        int line = (this->last_printed_line + i) % 4;

        // if the last alert's duration expired
        if (pros::millis() - this->line_set_time[line] >= this->screen_contents[line].duration) {
            // No alerts to print
            if (this->screen_buffer[line].size() == 0) {
                // text on screen is the same as last frame's text so no use updating
                if (this->screen_contents[line].text == this->next_print[line] && line != 3) {
                    this->next_print[line] = "";
                    continue;
                }
                // UPDATE TEXT/RUMBLE
                if (line == 3) this->controller.rumble(this->next_print[line].c_str());
                else this->controller.set_text(line, 0, this->next_print[line] + std::string(40, ' '));
                this->screen_contents[line].text = std::move(this->next_print[line]);
                this->next_print[line] = "";
            } else {
                // text on screen is the same as the alert's text so just set vars, dont update controller
                if (this->screen_contents[line].text == this->screen_buffer[line][0].text && line != 3) {
                    this->screen_contents[line] = this->screen_buffer[line][0];
                    this->screen_buffer[line].pop_front();
                    this->line_set_time[line] = pros::millis();
                    continue;
                }

                // SET ALERT/RUMBLE ALERT
                if (line == 3) this->controller.rumble(this->screen_buffer[line][0].text.c_str());
                else this->controller.set_text(line, 0, this->screen_buffer[line][0].text + std::string(40, ' '));
                this->screen_contents[line] = this->screen_buffer[line][0];
                this->screen_buffer[line].pop_front();
                this->line_set_time[line] = pros::millis();
            }
            this->last_printed_line = line;
            this->last_print_time = pros::millis();
        } else if (this->screen_contents[line].text == "") {
            // text is the same as last frame's text so no use updating
            if (this->screen_contents[line].text == this->next_print[line] && line != 3) {
                this->next_print[line] = "";
                continue;
            }

            // UPDATE TEXT/RUMBLE
            if (line == 3) this->controller.rumble(this->next_print[line].c_str());
            else this->controller.set_text(line, 0, this->next_print[line] + std::string(40, ' '));
            this->screen_contents[line].text = std::move(this->next_print[line]);
            this->next_print[line] = "";
            this->last_printed_line = line;
            this->last_print_time = pros::millis();
        }
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