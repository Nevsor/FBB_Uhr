#pragma once

#include <assert.h>
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
    StubClockCommandReader() = default;
    void add_command(ClockCommand const& command, int start_at_millis);
    ClockCommand read_command(uint8_t *) const;

private:
    ClockCommand commands[MAX_COMMANDS]; 
    unsigned int commands_timings_milliseconds[MAX_COMMANDS];
    int current_command = -1;
    int num_commands = 0;
};

template <int MAX_COMMANDS>
void StubClockCommandReader<MAX_COMMANDS>::add_command(
    ClockCommand const& command, 
    int start_at_millis) 
{
    assert (num_commands < MAX_COMMANDS);
    
    if (num_commands >= MAX_COMMANDS)
        return;
    
    commands[num_commands] = command;
    commands_timings_milliseconds[num_commands] = start_at_millis;

    num_commands++;
}

template <int MAX_COMMANDS>
ClockCommand StubClockCommandReader<MAX_COMMANDS>::read_command(uint8_t *) const {
    int &current_command = const_cast<int &>(this->current_command);

    unsigned long now = millis();

    while (
        current_command < num_commands-1
        && commands_timings_milliseconds[current_command+1] < now
    ) {
        current_command++;
    }

    return commands[current_command];
}
