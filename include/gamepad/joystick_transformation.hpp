#pragma once

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace gamepad {

/**
 * @brief An abstract class for joystick transformations.
 *
 * A transformation takes a coordinate representing the value of the joystick, and returns a transformed coordinate
 * value
 *
 */
class AbstractTransformation {
    public:
    /**
     * @brief Get the transformed coordinate given the original.
     *
     * @param original The original value of the joystick
     * @return std::pair<float, float> The transformed value
     */
        virtual std::pair<float, float> get_value(std::pair<float, float> original) = 0;
        virtual ~AbstractTransformation() = default;
};

/**
 * @brief A joystick transformation that applies a deadband to the joystick values
 *
 * A deadband makes the joystick value zero when the value is close to zero. This helps prevent drifting, since
 * joysticks often do not read exactly zero when released.
 */
class Deadband : public AbstractTransformation {
    public:
        /**
         * @brief Construct a new Deadband object
         *
         * @param x_deadband The deadband to apply for the x axis.
         * @param y_deadband The deadband to apply for the x axis.
         * @param x_spread How much the deadband for the x axis should widen.
         * @param y_spread How much the deadband for the y axis should widen.
         */
        Deadband(float x_deadband, float y_deadband, float x_spread, float y_spread)
            : m_x_deadband(x_deadband),
              m_y_deadband(y_deadband),
              m_x_spread(x_spread),
              m_y_spread(y_spread) {}

        /**
         * @brief Construct a new Deadband object
         *
         * @param x_deadband The deadband to apply for the x axis.
         * @param y_deadband The deadband to apply for the y axis.
         */
        Deadband(float x_deadband, float y_deadband)
            : Deadband(x_deadband, y_deadband, 0.0, 0.0) {}

        /**
         * @brief Get the joystick coordinate after applying the deadband
         *
         * @param original The value of the joystick before applying the deadband
         * @return std::pair<float, float> The joystick coordinate, with a deadband applied
         */
        std::pair<float, float> get_value(std::pair<float, float> original) override;
    private:
        float m_x_deadband;
        float m_y_deadband;
        float m_x_spread;
        float m_y_spread;
};

/**
 * @brief A joystick transformation that applies an expo curve to the joystick values
 *
 * An expo curve allows greater control of the joystick, by reducing the joystick values at low speeds, while still
 * allowing you to attain the maximum value of the joysticks.
 */
class ExpoCurve : public AbstractTransformation {
    public:
        /**
         * @brief Construct a new Expo Curve object
         *
         * @param x_curve How much the x axis should be curved. A higher value curves the joystick value more.
         * @param y_curve How much the y axis should be curved. A higher value curves the joystick value more.
         */
        ExpoCurve(float x_curve, float y_curve)
            : m_x_curve(x_curve),
              m_y_curve(y_curve) {}

        /**
         * @brief Get the joystick coordinate after applying the curve
         *
         * @param original The value of the joystick before applying the curve
         * @return std::pair<float, float> The joystick coordinate, with a curve applied
         */
        std::pair<float, float> get_value(std::pair<float, float> original) override;
    private:
        float m_x_curve;
        float m_y_curve;
};

/**
 * @brief A joystick transformation that applies a fisheye to the joystick values
 *
 * The vex controller joysticks don't reach their maximum value in the corners. This can be an issue, especially when
 * using single stick arcade. The fisheye "stretches" the joystick values so that they attain their maximum value even
 * in the corners of the joysticks.
 */
class Fisheye : public AbstractTransformation {
    public:
        /**
         * @brief Construct a new Fisheye object
         *
         * @param radius The radius of the rounded circle that forms the corners of the joystick's housing.
         */
        Fisheye(float radius)
            : m_radius(radius) {}

        /**
         * @brief Get the joystick coordinate after applying the fisheye
         *
         * @param original The value of the joystick before applying the fisheye
         * @return std::pair<float, float> The joystick coordinate, with a fisheye applied
         */
        std::pair<float, float> get_value(std::pair<float, float> original) override;
    private:
        float m_radius;
};

/**
 * @brief A chain of transformations. This class should not be directly used, but should be constructed using the
 * TransformationBuilder class.
 */
class Transformation final {
        friend class TransformationBuilder;
    public:
        std::pair<float, float> get_value(std::pair<float, float>);
    private:
        std::vector<std::unique_ptr<AbstractTransformation>> m_all_transforms;
};

/**
 * @brief A class to create a chain of transformations.
 *
 */
class TransformationBuilder final {
    public:
        /**
         * @brief Construct a new Transformation Builder object
         *
         * @param first The transformation that should be used first
         */
        template <std::derived_from<AbstractTransformation> T> TransformationBuilder(T first) {
            m_transform.m_all_transforms.push_back(std::make_unique<T>(std::move(first)));
        }

        TransformationBuilder() = delete;

        /**
         * @brief Add a transformation to the list of transformations to be applied.
         *
         * @param next The next transformation to be applied after the previous specified transformation
         * @return TransformationBuilder& The original Transformation Builder.
         */
        template <std::derived_from<AbstractTransformation> T> TransformationBuilder& and_then(T next) {
            m_transform.m_all_transforms.push_back(std::make_unique<T>(std::move(next)));
            return *this;
        }

        /**
         * @brief Generate the final chained transformation
         *
         * @return Transformation The final chained transformation. This can be passed to set_left_transform/set_right_transform
         */
        Transformation build() { return std::move(m_transform); }

        /**
         * @brief Generate the final chained transformation
         *
         * @return Transformation The final chained transformation. This can be passed to set_left_transform/set_right_transform
         */
        operator Transformation() { return std::move(m_transform); }
    private:
        Transformation m_transform {};
};
} // namespace gamepad
