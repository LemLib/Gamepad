#include "gamepad/controller.hpp"

namespace Gamepad {
void Controller::updateButton(pros::controller_digital_e_t button_id) {
    Button Controller::* button = Controller::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).rising_edge = !(this->*button).is_pressed && is_held;
    (this->*button).falling_edge = (this->*button).is_pressed && !is_held;
    (this->*button).is_pressed = is_held;
    if ((this->*button).rising_edge) {
        (this->*button).last_press_time = pros::millis();
    } 
    if ((this->*button).falling_edge) {
        (this->*button).last_release_time = pros::millis();
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
        default: std::exit(1); // TODO: change handling
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
        default: std::exit(1); // TODO: change handling
    }
}
}