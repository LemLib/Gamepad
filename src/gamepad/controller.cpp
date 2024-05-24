#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"

namespace Gamepad {

uint32_t Button::onPress(std::function<void(void)> func) const {
    return this->onPressEvent.add_listener(std::move(func));
}

uint32_t Button::onLongPress(std::function<void(void)> func) const {
    return this->onLongPressEvent.add_listener(std::move(func));
}

uint32_t Button::onRelease(std::function<void(void)> func) const {
    return this->onReleaseEvent.add_listener(std::move(func));
}

uint32_t Button::addListener(EventType event, std::function<void(void)> func) const {
    switch (event) {
        case Gamepad::EventType::ON_PRESS:
        this->onPress(std::move(func));
        case Gamepad::EventType::ON_LONG_PRESS:
        this->onLongPress(std::move(func));
        case Gamepad::EventType::ON_RELEASE:
        this->onRelease(std::move(func));
    }
}

bool Button::removeListener(uint32_t id) const {
    return this->onPressEvent.remove_listener(id) || this->onLongPressEvent.remove_listener(id) || this->onReleaseEvent.remove_listener(id);
}

void Button::update(const bool is_held) {
    this->rising_edge = !this->is_pressed && is_held;
    this->falling_edge = this->is_pressed && !is_held;
    this->is_pressed = is_held;
    if (is_held) {
        this->time_held += pros::millis() - this->last_update_time;
    } else {
        this->time_released += pros::millis() - this->last_update_time;
    }
    if (this->rising_edge) {
        this->time_held = 0;
    } 
    if (this->falling_edge) {
        this->time_released = 0;
    }

    if (this->rising_edge) {
        this->onPressEvent.fire();
    } else if (this->is_pressed && this->time_held >= this->long_press_threshold) {
        TODO("change onLongPress handling if onPress is present")
        this->onLongPressEvent.fire();
    }else if (this->falling_edge) {
        this->onReleaseEvent.fire();
    }
    this->last_update_time = pros::millis();
}

void Controller::updateButton(pros::controller_digital_e_t button_id) {
    Button Controller::* button = Controller::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
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