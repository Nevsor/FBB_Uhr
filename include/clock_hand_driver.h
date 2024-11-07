#pragma once

#include <clock_command.h>

class ClockHandDriver {

public:
    void set_zero();
    void set_command(ClockHandCommand command);
};