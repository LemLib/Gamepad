#include "gamepad/button.hpp"
#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"
#include "pros/misc.h"
#include <atomic>

namespace gamepad {
void Gamepad::updateButton(pros::controller_digital_e_t button_id) {
    Button Gamepad::*button = Gamepad::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Gamepad::update() {
    for (int i = pros::E_CONTROLLER_DIGITAL_L1; i <= pros::E_CONTROLLER_DIGITAL_A; ++i) {
        this->updateButton(static_cast<pros::controller_digital_e_t>(i));
    }

    this->m_LeftX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X) / 127.0;
    this->m_LeftY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) / 127.0;
    this->m_RightX = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X) / 127.0;
    this->m_RightY = this->controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) / 127.0;
}

const Button& Gamepad::operator[](pros::controller_digital_e_t button) { return this->*Gamepad::button_to_ptr(button); }

float Gamepad::operator[](pros::controller_analog_e_t axis) {
    switch (axis) {
        case pros::E_CONTROLLER_ANALOG_LEFT_X: return m_LeftX;
        case pros::E_CONTROLLER_ANALOG_LEFT_Y: return m_LeftY;
        case pros::E_CONTROLLER_ANALOG_RIGHT_X: return m_RightX;
        case pros::E_CONTROLLER_ANALOG_RIGHT_Y: return m_RightY;
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

Button Gamepad::*Gamepad::button_to_ptr(pros::controller_digital_e_t button) {
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
