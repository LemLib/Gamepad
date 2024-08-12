#include <array>
#include <optional>
#include <set>
#include <string>

#include "pros/misc.h"


namespace Gamepad {
    
typedef std::array<std::optional<std::string>, 3> ScreenBuffer;

class AbstractScreen {
    public:
        AbstractScreen(uint priority): priority(priority) {}
        virtual ScreenBuffer get_screen(std::set<uint8_t> visible_lines) = 0;
        virtual void handle_events(std::set<pros::controller_digital_e_t> button_events) = 0;
        uint get_priority();
    private:
        uint priority;
};

} // namespace Gamepad