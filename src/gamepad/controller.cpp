#include "gamepad/controller.hpp"
#include "gamepad/todo.hpp"

namespace Gamepad {

bool Button::onPress(std::string listenerName, std::function<void(void)> func) const {
    return this->onPressEvent.add_listener(std::move(listenerName), std::move(func));
}

bool Button::onLongPress(std::string listenerName, std::function<void(void)> func) const {
    return this->onLongPressEvent.add_listener(std::move(listenerName), std::move(func));
}

bool Button::onRelease(std::string listenerName, std::function<void(void)> func) const {
    return this->onReleaseEvent.add_listener(std::move(listenerName), std::move(func));
}

bool Button::onShortRelease(std::string listenerName, std::function<void(void)> func) const {
    return this->onShortReleaseEvent.add_listener(std::move(listenerName), std::move(func));
}

bool Button::addListener(EventType event, std::string listenerName, std::function<void(void)> func) const {
    switch (event) {
        case Gamepad::EventType::ON_PRESS: return this->onPress(std::move(listenerName), std::move(func));
        case Gamepad::EventType::ON_LONG_PRESS: return this->onLongPress(std::move(listenerName), std::move(func));
        case Gamepad::EventType::ON_RELEASE: return this->onRelease(std::move(listenerName), std::move(func));
        case Gamepad::EventType::ON_SHORT_RELEASE:
            return this->onShortRelease(std::move(listenerName), std::move(func));
        default:
            TODO("add error logging")
            errno = EINVAL;
            return false;
    }
}

bool Button::removeListener(std::string listenerName) const {
    return this->onPressEvent.remove_listener(listenerName) || this->onLongPressEvent.remove_listener(listenerName) ||
           this->onReleaseEvent.remove_listener(listenerName) ||
           this->onShortReleaseEvent.remove_listener(listenerName);
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
        this->onPressEvent.fire();
    } else if (this->is_pressed && this->time_held >= this->long_press_threshold &&
               this->last_long_press_time <= pros::millis() - this->time_held) {
        TODO("change onLongPress handling if onPress is present")
        this->onLongPressEvent.fire();
        this->last_long_press_time = pros::millis();
    } else if (this->falling_edge) {
        this->onReleaseEvent.fire();
        if (this->time_held < this->long_press_threshold) { this->onShortReleaseEvent.fire(); }
    }
    if (this->rising_edge) { this->time_held = 0; }
    if (this->falling_edge) { this->time_released = 0; }
    this->last_update_time = pros::millis();
}

void Controller::updateButton(pros::controller_digital_e_t button_id) {
    Button Controller::*button = Controller::button_to_ptr(button_id);
    bool is_held = this->controller.get_digital(button_id);
    (this->*button).update(is_held);
}

void Controller::update() {
    for (int i = DIGITAL_L1; i != DIGITAL_A; ++i) { this->updateButton(static_cast<pros::controller_digital_e_t>(i)); }

    this->m_LeftX = this->controller.get_analog(ANALOG_LEFT_X);
    this->m_LeftY = this->controller.get_analog(ANALOG_LEFT_Y);
    this->m_RightX = this->controller.get_analog(ANALOG_RIGHT_X);
    this->m_RightY = this->controller.get_analog(ANALOG_RIGHT_Y);
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
        default:
            TODO("add error logging")
            errno = EINVAL;
            return 0;
    }
}

Button Controller::*Controller::button_to_ptr(pros::controller_digital_e_t button) {
    switch (button) {
        case DIGITAL_L1: return &Controller::m_L1;
        case DIGITAL_L2: return &Controller::m_L2;
        case DIGITAL_R1: return &Controller::m_R1;
        case DIGITAL_R2: return &Controller::m_R2;
        case DIGITAL_UP: return &Controller::m_Up;
        case DIGITAL_DOWN: return &Controller::m_Down;
        case DIGITAL_LEFT: return &Controller::m_Left;
        case DIGITAL_RIGHT: return &Controller::m_Right;
        case DIGITAL_X: return &Controller::m_X;
        case DIGITAL_B: return &Controller::m_B;
        case DIGITAL_Y: return &Controller::m_Y;
        case DIGITAL_A: return &Controller::m_A;
        default:
            TODO("add error logging")
            errno = EINVAL;
            return &Controller::Fake;
    }
}
} // namespace Gamepad