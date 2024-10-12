#pragma once

#include "pros/misc.h"
#include "screens/defaultScreen.hpp"
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <sys/types.h>
#include "screens/abstractScreen.hpp"
#include "button.hpp"
#include "pros/misc.hpp"

namespace gamepad {
class Gamepad {
    public:
        /**
         * @brief Updates the state of the gamepad (all joysticks and buttons), and also runs
         * any registered listeners.
         *
         * @note This function should be called at the beginning of every loop iteration.
         *
         * @b Example:
         * @code {.cpp}
         * while (true) {
         *   gamepad::master.update();
         *   // do robot control stuff here...
         *   pros::delay(25);
         * }
         * @endcode
         *
         */
        void update();
        /**
         * Add a screen to the sceen update loop that can update the controller's screen
         *
         * @param screen the `AbstractScreen` to add to the screen queue
         */
        void add_screen(std::shared_ptr<AbstractScreen> screen);
        /**
         * print a line to the console like pros (low priority)
         *
         * @param line the line number to print the string on (0-2)
         * @param str the string to print onto the controller (\n to go to the next line)
         */
        void print_line(uint8_t line, std::string str);
        /**
         * @brief clears all lines on the controller, similar to the pros function (low priority)
         * 
         */
        void clear();
        /**
         * @brief clears the specific line on the controller, similar to the pros function clear_line (low priority)
         * 
         * @param line the line to clear (0-2)
         */
        void clear(uint8_t line);
        /**
         * makes the controller rumble like pros (low priority)
         *
         * @param rumble_pattern A string consisting of the characters '.', '-', and ' ', where dots are short rumbles,
         * dashes are long rumbles, and spaces are pauses. Maximum supported length is 8 characters.
         */
        void rumble(std::string rumble_pattern);
        /**
         * @brief Get the state of a button on the controller.
         *
         * @param button Which button to return
         *
         * @b Example:
         * @code {.cpp}
         * if(gamepad::master[DIGITAL_L1]) {
         *   // do something here...
         * }
         * @endcode
         *
         */
        const Button& operator[](pros::controller_digital_e_t button);
        /**
         * @brief Get the value of a joystick axis on the controller.
         *
         * @param joystick Which joystick axis to return
         *
         * @b Example:
         * @code {.cpp}
         * // control a motor with a joystick
         * intake.move(gamepad::master[ANALOG_RIGHT_Y]);
         * @endcode
         *
         */
        float operator[](pros::controller_analog_e_t joystick);
        const Button& L1 {m_L1};
        const Button& L2 {m_L2};
        const Button& R1 {m_R1};
        const Button& R2 {m_R2};
        const Button& Up {m_Up};
        const Button& Down {m_Down};
        const Button& Left {m_Left};
        const Button& Right {m_Right};
        const Button& X {m_X};
        const Button& B {m_B};
        const Button& Y {m_Y};
        const Button& A {m_A};
        const float& LeftX = m_LeftX;
        const float& LeftY = m_LeftY;
        const float& RightX = m_RightX;
        const float& RightY = m_RightY;
        /// The master controller, same as @ref gamepad::master
        static Gamepad master;
        /// The partner controller, same as @ref gamepad::partner
        static Gamepad partner;
    private:
        Gamepad(pros::controller_id_e_t id)
            : controller(id) {
            screens.push_back(defaultScreen);
        }

        Button m_L1 {}, m_L2 {}, m_R1 {}, m_R2 {}, m_Up {}, m_Down {}, m_Left {}, m_Right {}, m_X {}, m_B {}, m_Y {},
            m_A {};
        float m_LeftX = 0, m_LeftY = 0, m_RightX = 0, m_RightY = 0;
        Button Fake {};
        /**
         * @brief Gets a unique name for a listener that will not conflict with user listener names.
         *
         * @important: when using the function, you must register the listener by
         * directly calling add_listener on the EventHandler, do NOT use onPress/addListener,etc.
         *
         * @return std::string A unique listener name
         */
        static std::string unique_name();
        static Button Gamepad::* button_to_ptr(pros::controller_digital_e_t button);
        void updateButton(pros::controller_digital_e_t button_id);

        void updateScreens();

        std::shared_ptr<DefaultScreen> defaultScreen = std::make_shared<DefaultScreen>();
        std::vector<std::shared_ptr<AbstractScreen>> screens {};
        ScreenBuffer currentScreen {};
        ScreenBuffer nextBuffer {};
        pros::Controller controller;

        uint8_t last_printed_line = 0;
        uint last_print_time = 0;
        uint last_update_time = 0;
        bool screenCleared = true;
        pros::Mutex mut {};
};

inline Gamepad Gamepad::master {pros::E_CONTROLLER_MASTER};
inline Gamepad Gamepad::partner {pros::E_CONTROLLER_PARTNER};
/// The master controller
inline Gamepad& master = Gamepad::master;
/// The partner controller
inline Gamepad& partner = Gamepad::partner;

} // namespace gamepad
