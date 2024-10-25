#include "gamepad/drive_curve.hpp"
#include <cmath>

namespace Gamepad {
ExpoDriveCurve::ExpoDriveCurve(float minOutput, float curve) :
      minOutput(minOutput),
      curveGain(curve) {}

float ExpoDriveCurve::curve(float input) {
    // g is the output of g(x) as defined in the Desmos graph
    const float g = std::abs(input);
    // i is the output of i(x) as defined in the Desmos graph
    const float i = std::copysign(std::pow(curveGain, g - 127) * g, input);
    return (127.0 - minOutput) / (127) * i + std::copysign(minOutput, input);
}
} // namespace Gamepad