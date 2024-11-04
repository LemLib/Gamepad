#pragma once

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace gamepad {
class AbstractTransformation {
    public:
        virtual std::pair<float, float> get_value(std::pair<float, float>) = 0;
        virtual ~AbstractTransformation() = default;
};

class Deadband : public AbstractTransformation {
    public:
        Deadband(float x_deadband, float y_deadband, float x_spread, float y_spread)
            : m_x_deadband(x_deadband),
              m_y_deadband(y_deadband),
              m_x_spread(x_spread),
              m_y_spread(y_spread) {}

        Deadband(float x_deadband, float y_deadband)
            : Deadband(x_deadband, y_deadband, 0.0, 0.0) {}

        std::pair<float, float> get_value(std::pair<float, float>) override;
    private:
        float m_x_deadband;
        float m_y_deadband;
        float m_x_spread;
        float m_y_spread;
};

class ExpoCurve : public AbstractTransformation {
    public:
        ExpoCurve(float x_curve, float y_curve)
            : m_x_curve(x_curve),
              m_y_curve(y_curve) {}

        std::pair<float, float> get_value(std::pair<float, float>) override;
    private:
        float m_x_curve;
        float m_y_curve;
};

class Fisheye : public AbstractTransformation {
    public:
        Fisheye(float radius)
            : m_radius(radius) {}

        std::pair<float, float> get_value(std::pair<float, float>) override;
    private:
        float m_radius;
};

class Transformation final {
        friend class TransformationBuilder;
    public:
        std::pair<float, float> get_value(std::pair<float, float>);
    private:
        std::vector<std::unique_ptr<AbstractTransformation>> m_all_transforms;
};

class TransformationBuilder final {
    public:
        template <std::derived_from<AbstractTransformation> T> TransformationBuilder(T first) {
            m_transform.m_all_transforms.push_back(std::make_unique<T>(std::move(first)));
        }

        TransformationBuilder() = delete;

        template <std::derived_from<AbstractTransformation> T> TransformationBuilder& and_then(T next) {
            m_transform.m_all_transforms.push_back(std::make_unique<T>(std::move(next)));
            return *this;
        }

        Transformation build() { return std::move(m_transform); }

        operator Transformation() { return std::move(m_transform); }
    private:
        Transformation m_transform {};
};
} // namespace gamepad
