#include <memory>

#include "bindings.hpp"
#include "controller.hpp"

namespace Gamepad::Bindings {
bool motorTwoButton(std::string name, pros::Motor motor, Button& fwd_button, Button& rev_button) {
    enum states { FWD, REV, NONE };

    bool ret_val = true;
    ret_val &= !fwd_button.hasListener(EventType::ON_PRESS, "Bindings" + name);
    ret_val &= !fwd_button.hasListener(EventType::ON_RELEASE, "Bindings" + name);
    ret_val &= !rev_button.hasListener(EventType::ON_PRESS, "Bindings" + name);
    ret_val &= !rev_button.hasListener(EventType::ON_RELEASE, "Bindings" + name);
    if (!ret_val) return false;

    auto state = std::make_shared<states>(NONE);
    fwd_button.onPress("Bindings" + name, [=]() {
        *state = FWD;
        motor.move(127);
    });
    fwd_button.onRelease("Bindings" + name, [=]() {
        if (*state == FWD) {
            *state = NONE;
            motor.move(0);
        }
    });
    rev_button.onPress("Bindings" + name, [=]() {
        *state = REV;
        motor.move(-127);
    });
    rev_button.onRelease("Bindings" + name, [=]() {
        if (*state == REV) {
            *state = NONE;
            motor.move(0);
        }
    });
    return true;
}

bool motorOneButton(std::string name, pros::Motor motor, Button& toggle_button, uint32_t speed) {
    bool ret_val = true;
    ret_val &= !toggle_button.hasListener(EventType::ON_PRESS, "Bindings" + name);
    if (!ret_val) return false;
    auto state = std::make_shared<bool>(false);
    ret_val &= toggle_button.onPress("Bindings" + name, [=]() {
        if (*state) motor.move(speed);
        else motor.move(0);
        *state = !*state;
    });
    return true;
}
} // namespace Gamepad::Bindings