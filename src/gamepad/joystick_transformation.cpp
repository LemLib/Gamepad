#include "joystick_transformation.hpp"
#include <cmath>
#include <numeric>

using std::abs;
using std::copysign;
using std::pow;

namespace gamepad {
std::pair<float, float> Deadband::get_value(std::pair<float, float> value) {
    float x = value.first;
    float y = value.second;
    float x_deadband = m_x_deadband + abs(y) * m_x_spread;
    float y_deadband = m_y_deadband + abs(x) * m_y_spread;
    float x_scale = 1.0 / (1.0 - x_deadband);
    float y_scale = 1.0 / (1.0 - y_deadband);
    x = copysign(abs(x) < x_deadband ? 0 : (abs(x) - x_deadband) * x_scale, x);
    y = copysign(abs(y) < y_deadband ? 0 : (abs(y) - y_deadband) * y_scale, y);
    return {x, y};
}

std::pair<float, float> ExpoCurve::get_value(std::pair<float, float> value) {
    float x = value.first;
    float y = value.second;
    x = copysign(pow(abs(x), m_x_curve), x);
    y = copysign(pow(abs(y), m_y_curve), y);
    return {x, y};
}

std::pair<float, float> Fisheye::get_value(std::pair<float, float> value) {
    float x = value.first;
    float y = value.second;
    float x_abs = abs(x);
    float y_abs = abs(y);
    float j = std::sqrt(m_radius * m_radius - 1.0 * 1.0);
    if (x_abs >= j && y_abs >= j) {
        float scale = std::hypot(std::min(x_abs / y_abs, y_abs / x_abs), 1.0) / m_radius;
        x_abs *= scale;
        y_abs *= scale;
    }
    x = std::copysign(std::min(1.0f, x_abs), x);
    y = std::copysign(std::min(1.0f, y_abs), y);
    return {x, y};
}

std::pair<float, float> Transformation::get_value(std::pair<float, float> value) {
    return std::accumulate(m_all_transforms.begin(), m_all_transforms.end(), value,
                           [](auto last_val, auto& next_transform) { return next_transform->get_value(last_val); });
}
} // namespace gamepad