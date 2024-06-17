#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"

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

void Controller::update() {
    for (int i = DIGITAL_L1; i != DIGITAL_A; ++i) { this->updateButton(static_cast<pros::controller_digital_e_t>(i)); }

    this->LeftX = this->controller.get_analog(ANALOG_LEFT_X);
    if (this->CurveLeftX) this->LeftX = this->CurveLeftX->curve(this->LeftX);
    this->LeftY = this->controller.get_analog(ANALOG_LEFT_Y);
    if (this->CurveLeftY) this->LeftY = this->CurveLeftY->curve(this->LeftY);
    this->RightX = this->controller.get_analog(ANALOG_RIGHT_X);
    if (this->CurveRightX) this->RightX = this->CurveRightX->curve(this->RightX);
    this->RightY = this->controller.get_analog(ANALOG_RIGHT_Y);
    if (this->CurveRightY) this->RightY = this->CurveRightY->curve(this->RightY);
}

const Button& Controller::operator[](pros::controller_digital_e_t button) {
    return this->*Controller::button_to_ptr(button);
}

float Controller::operator[](pros::controller_analog_e_t axis) {
    switch (axis) {
        case ANALOG_LEFT_X: return this->LeftX;
        case ANALOG_LEFT_Y: return this->LeftY;
        case ANALOG_RIGHT_X: return this->RightX;
        case ANALOG_RIGHT_Y: return this->RightY; TODO("change handling for default")
        default: std::exit(1);
    }
}

void Controller::setCurve(pros::controller_analog_e_t joystick, std::shared_ptr<DriveCurve> curve) {
    switch (axis) {
        case ANALOG_LEFT_X: this->CurveLeftX = curve;
        case ANALOG_LEFT_Y: this->CurveLeftY = curve;
        case ANALOG_RIGHT_X: this->CurveRightX = curve;
        case ANALOG_RIGHT_Y: this->CurveRightY = curve; TODO("add error logging")
        default: errno = EINVAL;
    }
}

void Controller::setCurve(pros::controller_analog_e_t joystick, DriveCurve& curve) {
    this->setCurve(joystick, std::shared_ptr<DriveCurve>(&curve, [](void*) {}));
}

Button Controller::*Controller::button_to_ptr(pros::controller_digital_e_t button) {
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
        case DIGITAL_A: return &Controller::A; TODO("change handling for default")
        default: std::exit(1);
    }
}
} // namespace Gamepad