#pragma once

#include <stdint.h>

#include <clock_command.h>
#include <dmx_source.h>


// Parst `ClockCommand`s aus einem Byte-Array.
class DmxClockCommandReader {
public:
    ClockCommand read_command(uint8_t *data) const;
};

// Stub-Klasse um eine Sequenz von fest programmierten `ClockCommand`s auf
// der Uhr zu testen.
template <int MAX_COMMANDS>
class StubClockCommandReader {
public:
    void add_command(ClockCommand const& command, int start_at_millis);
    ClockCommand read_command(uint8_t *) const;

private:
    ClockCommand commands[MAX_COMMANDS]; 
    int commands_timings_milliseconds[MAX_COMMANDS];
    int current_command = -1;
    int num_commands = 0;
};
