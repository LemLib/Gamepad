#include <memory>

#include "bindings.hpp"
#include "controller.hpp"

namespace Gamepad::Bindings {
bool motorTwoButton(std::string name, pros::Motor motor, Button& fwd_button, Button& rev_button) {
    enum states { FWD, REV, NONE };

    bool ret_val = true;
    auto state = std::make_shared<states>(NONE);
    ret_val &= fwd_button.onPress("Bindings" + name, [=]() {
        *state = FWD;
        motor.move(127);
    });
    ret_val &= fwd_button.onRelease("Bindings" + name, [=]() {
        if (*state == FWD) {
            *state = NONE;
            motor.move(0);
        }
    });
    ret_val &= rev_button.onPress("Bindings" + name, [=]() {
        *state = REV;
        motor.move(-127);
    });
    ret_val &= rev_button.onRelease("Bindings" + name, [=]() {
        if (*state == REV) {
            *state = NONE;
            motor.move(0);
        }
    });
    return ret_val;
}

bool motorOneButton(std::string name, pros::Motor motor, Button& toggle_button, uint32_t speed) {
    bool ret_val = true;
    auto state = std::make_shared<bool>(false);
    ret_val &= toggle_button.onPress("Bindings" + name, [=]() {
        if (*state) motor.move(speed);
        else motor.move(0);
        *state = !*state;
    });
    return ret_val;
}
} // namespace Gamepad::Bindings