#include "gamepad/button.hpp"
#include "gamepad/todo.hpp"
#include "pros/rtos.hpp"
#include <cstdint>

namespace gamepad {
void Button::setLongPressThreshold(uint32_t threshold) const { this->long_press_threshold = threshold; }

void Button::setRepeatCooldown(uint32_t cooldown) const { this->repeat_cooldown = cooldown; }

bool Button::onPress(std::string listenerName, std::function<void(void)> func) const {
    return this->on_press_event.addListener(std::move(listenerName) + "_user", std::move(func));
}

bool Button::onLongPress(std::string listenerName, std::function<void(void)> func) const {
    return this->on_long_press_event.addListener(std::move(listenerName) + "_user", std::move(func));
}

bool Button::onRelease(std::string listenerName, std::function<void(void)> func) const {
    return this->on_release_event.addListener(std::move(listenerName) + "_user", std::move(func));
}

bool Button::onShortRelease(std::string listenerName, std::function<void(void)> func) const {
    return this->on_short_release_event.addListener(std::move(listenerName) + "_user", std::move(func));
}

bool Button::onLongRelease(std::string listenerName, std::function<void(void)> func) const {
    return this->on_long_release_event.addListener(std::move(listenerName) + "_user", std::move(func));
}

bool Button::onRepeatPress(std::string listenerName, std::function<void(void)> func) const {
    return this->on_repeat_press_event.addListener(std::move(listenerName) + "_user", std::move(func));
}

bool Button::addListener(EventType event, std::string listenerName, std::function<void(void)> func) const {
    switch (event) {
        case gamepad::EventType::ON_PRESS: return this->onPress(std::move(listenerName), std::move(func));
        case gamepad::EventType::ON_LONG_PRESS: return this->onLongPress(std::move(listenerName), std::move(func));
        case gamepad::EventType::ON_RELEASE: return this->onRelease(std::move(listenerName), std::move(func));
        case gamepad::EventType::ON_SHORT_RELEASE:
            return this->onShortRelease(std::move(listenerName), std::move(func));
        case gamepad::EventType::ON_LONG_RELEASE: return this->onLongRelease(std::move(listenerName), std::move(func));
        case gamepad::EventType::ON_REPEAT_PRESS: return this->onRepeatPress(std::move(listenerName), std::move(func));
        default:
            TODO("add error logging")
            errno = EINVAL;
            return false;
    }
}

bool Button::removeListener(std::string listenerName) const {
    return this->on_press_event.removeListener(listenerName + "_user") ||
           this->on_long_press_event.removeListener(listenerName + "_user") ||
           this->on_release_event.removeListener(listenerName + "_user") ||
           this->on_short_release_event.removeListener(listenerName + "_user") ||
           this->on_long_release_event.removeListener(listenerName + "_user") ||
           this->on_repeat_press_event.removeListener(listenerName + "_user");
}

void Button::update(const bool is_held) {
    this->rising_edge = !this->is_pressed && is_held;
    this->falling_edge = this->is_pressed && !is_held;
    this->is_pressed = is_held;
    if (is_held) this->time_held += pros::millis() - this->last_update_time;
    else this->time_released += pros::millis() - this->last_update_time;

    if (this->rising_edge) {
        this->on_press_event.fire();
    } else if (this->is_pressed && this->time_held >= this->long_press_threshold &&
               this->last_long_press_time <= pros::millis() - this->time_held) {
        this->on_long_press_event.fire();
        this->last_long_press_time = pros::millis();
        this->last_repeat_time = pros::millis() - this->repeat_cooldown;
        this->repeat_iterations = 0;
    } else if (this->is_pressed && this->time_held >= this->long_press_threshold &&
               pros::millis() - this->last_repeat_time >= this->repeat_cooldown) {
        this->repeat_iterations++;
        this->on_repeat_press_event.fire();
        this->last_repeat_time = pros::millis();
    } else if (this->falling_edge) {
        this->on_release_event.fire();
        if (this->time_held < this->long_press_threshold) this->on_short_release_event.fire();
        else this->on_long_release_event.fire();
    }

    if (this->rising_edge) this->time_held = 0;
    if (this->falling_edge) this->time_released = 0;
    this->last_update_time = pros::millis();
}
} // namespace gamepad
