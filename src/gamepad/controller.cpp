#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"
#include "pros/misc.h"
#include <atomic>

namespace Gamepad {

void Controller::updateButton(pros::controller_digital_e_t button_id) {
    Button Controller::*button = Controller::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Controller::update() {
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i <= pros::E_CONTROLLER_DIGITAL_A; ++i) {
        this->updateButton(static_cast<pros::controller_digital_e_t>(i));
    }

    this->m_LeftX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    this->m_LeftY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    this->m_RightX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    this->m_RightY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
}

const Button& Controller::operator[](pros::controller_digital_e_t button) {
    return this->*Controller::button_to_ptr(button);
}

float Controller::operator[](pros::controller_analog_e_t axis) {
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

std::string Controller::unique_name() {
    static std::atomic<uint32_t> i = 0;
    return std::to_string(i++) + "_internal";
}

Button Controller::*Controller::button_to_ptr(pros::controller_digital_e_t button) {
    switch (button) {
        case pros::E_CONTROLLER_DIGITAL_L1: return &Controller::m_L1;
        case pros::E_CONTROLLER_DIGITAL_L2: return &Controller::m_L2;
        case pros::E_CONTROLLER_DIGITAL_R1: return &Controller::m_R1;
        case pros::E_CONTROLLER_DIGITAL_R2: return &Controller::m_R2;
        case pros::E_CONTROLLER_DIGITAL_UP: return &Controller::m_Up;
        case pros::E_CONTROLLER_DIGITAL_DOWN: return &Controller::m_Down;
        case pros::E_CONTROLLER_DIGITAL_LEFT: return &Controller::m_Left;
        case pros::E_CONTROLLER_DIGITAL_RIGHT: return &Controller::m_Right;
        case pros::E_CONTROLLER_DIGITAL_X: return &Controller::m_X;
        case pros::E_CONTROLLER_DIGITAL_B: return &Controller::m_B;
        case pros::E_CONTROLLER_DIGITAL_Y: return &Controller::m_Y;
        case pros::E_CONTROLLER_DIGITAL_A: return &Controller::m_A;
        default:
            TODO("add error logging")
            errno = EINVAL;
            return &Controller::Fake;
    }
}
} // namespace Gamepad
