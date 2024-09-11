#pragma once

#include "controller.hpp"

namespace Gamepad::Bindings {

bool motorTwoButton(std::string name, pros::Motor motor, Button& fwd_button, Button& rev_button);
bool motorOneButton(std::string name, pros::Motor motor, Button& toggle_button, uint32_t speed = 127);
} // namespace Gamepad::Bindings