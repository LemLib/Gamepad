#include "main.h"
#include "gamepad/api.hpp"
#include "gamepad/gamepad.hpp"
#include "gamepad/screens/alertScreen.hpp"
#include "pros/rtos.hpp"
#include <cstdio>
#include <memory>
#include <string>

// initialize the alerts screen so we can have alerts on the controller
std::shared_ptr<gamepad::AlertScreen> alerts = std::make_shared<gamepad::AlertScreen>();

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    // VERY IMPORTANT, this actually adds the alerts screen to the gamepad
    // it wouldn't work without this line
    gamepad::master.addScreen(alerts);

    // When the A button is pressed, schedule an alert that spans all three
    // lines, lasts 3 seconds and rumbles in a long-short-long pattern
    gamepad::master.buttonA().onPress("alert", []() {
        alerts->addAlerts(0, "a very\nimportant alert\nat " + std::to_string(pros::millis()) + " ms", 3000, "-.-");
    });

    // Normally print a string on the first and third line without overriding
    // the second line when the B button is pressed
    gamepad::master.buttonB().onPress(
        "print02", []() { gamepad::master.printLine(0, "the time is\n\n" + std::to_string(pros::millis()) + " ms"); });

    // rumbles 3 times for a short duration when the X button is pressed
    gamepad::master.buttonX().onPress("rumble", []() { gamepad::master.rumble("..."); });

    // when the Y button is pressed and held the text should show up, and when
    // the button is released it should be cleared
    gamepad::master.buttonY().onPress("print1", []() { gamepad::master.printLine(1, "this should be cleared"); });
    gamepad::master.buttonY().onRelease("clear1", []() { gamepad::master.clear(1); });

    // set up controller curves:
    gamepad::master.set_left_transform(
        gamepad::TransformationBuilder(gamepad::Deadband(0.05, 0.05)).and_then(gamepad::ExpoCurve(2, 2)));
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
    pros::MotorGroup left_mg({1, -2, 3}); // Creates a motor group with forwards ports 1 & 3 and reversed port 2
    pros::MotorGroup right_mg({-4, 5, -6}); // Creates a motor group with forwards port 4 and reversed ports 4 & 6
    while (true) {
        // Remember to ALWAYS call update at the start of your while loop!
        gamepad::master.update();
        // We'll use the arcade control scheme
        int dir = gamepad::master.axisLeftY() * 127; // Gets amount forward/backward from left joystick
        int turn = gamepad::master.axisRightX() * 127; // Gets the turn left/right from right joystick
        left_mg.move(dir - turn); // Sets left motor voltage
        right_mg.move(dir + turn); // Sets right motor voltage
        pros::delay(25); // Wait for 25 ms, then update the motor values again
    }
}
