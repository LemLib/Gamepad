#include "gamepad/controller.hpp"

namespace Gamepad {
void Controller::updateButton(ButtonState Controller::* button, bool is_held) {
    this->*button.rising_edge = !this->*button.is_pressed && is_held;
    this->*button.falling_edge = this->*button.is_pressed && !is_held;
    this->*button.is_pressed = is_held;
    if (this->*button.rising_edge) {
        this->*button.last_press_time = pros::millis();
    } 
    if (this->*button.falling_edge) {
        this->*button.last_release_time = pros::millis();
    }
}

void Controller::update() {
    this->updateButton(&Controller::L1);
    this->updateButton(&Controller::L2);
    this->updateButton(&Controller::R1);
    this->updateButton(&Controller::R2);
    this->updateButton(&Controller::Up);
    this->updateButton(&Controller::Down);
    this->updateButton(&Controller::Left);
    this->updateButton(&Controller::Right);
    this->updateButton(&Controller::X);
    this->updateButton(&Controller::B);
    this->updateButton(&Controller::Y);
    this->updateButton(&Controller::A);

    this->LeftX = this->controller.get_analog(ANALOG_LEFT_X);
    this->LeftY = this->controller.get_analog(ANALOG_LEFT_Y);
    this->RightX = this->controller.get_analog(ANALOG_RIGHT_X);
    this->RightY = this->controller.get_analog(ANALOG_RIGHT_Y);
}

ButtonState Controller::operator[](pros::controller_digital_e_t button) {
    switch (button) {
        case DIGITAL_L1: return this->L1;
        case DIGITAL_L2: return this->L2;
        case DIGITAL_R1: return this->R1;
        case DIGITAL_R2: return this->R2;
        case DIGITAL_UP: return this->Up;
        case DIGITAL_DOWN: return this->Down;
        case DIGITAL_LEFT: return this->Left;
        case DIGITAL_RIGHT: return this->Right;
        case DIGITAL_X: return this->X;
        case DIGITAL_B: return this->B;
        case DIGITAL_Y: return this->Y;
        case DIGITAL_A: return this->A;
        default: std::exit(1); // TODO: change handling
    }
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
}