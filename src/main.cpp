#include "main.h"
#include "gamepad/api.hpp"
#include "gamepad/controller.hpp"
#include "pros/rtos.hpp"
#include <cstdint>
#include <cstdio>

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */

uint32_t last_repeat_press_time = pros::millis();

void downPress1() { printf("Down Press!\n"); }

void upRelease1() { printf("Up Release!\n"); }

void leftLongPress1() { printf("Left Long Press!\n"); }

void leftShortRelease1() { printf("Left Short Release!\n"); }

void leftLongRelease1() { printf("Left Long Release!\n"); }

void aPress1() {
    last_repeat_press_time = pros::millis();
    printf("A Pressed!\n");
}

void aRepeatPress1() {
    printf("A Repeat Pressed %ims after last\n", pros::millis() - last_repeat_press_time);
    last_repeat_press_time = pros::millis();
}

void initialize() {
    // We can register functions to run when buttons are pressed
    gamepad::master.Down().onPress("downPress1", downPress1);
    // ...or when they're released
    gamepad::master.Up().onRelease("downRelease1", upRelease1);
    // There's also the longPress event
    gamepad::master.Left().onLongPress("leftLongPress1", leftLongPress1);
    // We can have two or even more functions on one button,
    // just remember to give them different names
    gamepad::master.Left().onShortRelease("leftShortRelease", leftShortRelease1);
    gamepad::master.Left().onLongRelease("leftLongRelease", leftLongRelease1);
    // We also have the repeat press event, where we can adjust the timing
    gamepad::master.A().set_long_press_threshold(1000); // in ms
    gamepad::master.A().set_repeat_cooldown(100); // in ms
    gamepad::master.A().onPress("aStartPress", aPress1);
    gamepad::master.A().onRepeatPress("aRepeatPress", aRepeatPress1);
    // And we can use lambda's too
    gamepad::master.X().onShortRelease("xShortRelease1", []() { printf("X Short Release!\n"); });
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
        int dir = gamepad::master.LeftY(); // Gets amount forward/backward from left joystick
        int turn = gamepad::master.RightX(); // Gets the turn left/right from right joystick
        left_mg.move(dir - turn); // Sets left motor voltage
        right_mg.move(dir + turn); // Sets right motor voltage
        pros::delay(25); // Wait for 25 ms, then update the motor values again
    }
}