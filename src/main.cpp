#include "main.h"
#include "gamepad/api.hpp"
#include "pros/screen.hpp"

Gamepad::Controller master(CONTROLLER_MASTER);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {}

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
    auto i = master.Down.onPress("downPress1", []() { printf("Down Press!\n"); });
    if (!i) std::cout << "opcontrol ran again, event did not register!" << std::endl;
    master.Down.onLongPress("downLongPress1", []() { printf("Down longPress!\n"); });
    master.Down.onRelease("downRelease1", []() { printf("Down Release!\n"); });
    master.Up.onPress("uppress1", []() { printf("Up Press!\n"); });
    master.Up.onLongPress("upLongPress1", []() { printf("Up longPress!\n"); });
    master.Up.onRelease("upRelease1", []() { printf("Up Release!\n"); });
    master.Up.onShortRelease("upShortRelease1", []() { printf("Up Short Release!\n"); });
    master.Up.onRelease("upRelease2", [=]() {
        master.Up.removeListener("upRelease1");
        printf("Up Release 2!\n");
    });
    while (true) {
        master.update();
        pros::screen::print(TEXT_MEDIUM, 3, "%f %f %f %f", master.LeftX, master.LeftY, master.RightX, master.RightY);
        pros::delay(20);
    }
}