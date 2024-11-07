#pragma once

#include <clock_command.h>

/// This has to be called before clock commands can be received.
void setup_dmx();

/// Read the command the master is currently sending.
ClockCommand read_clock_command();