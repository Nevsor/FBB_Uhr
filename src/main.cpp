#include <Arduino.h>

#include <configuration.h>
#include <clock_driver.h>
#include <clock_command_reader.h>
#include <dmx_source.h>
#include <run_tests.h>
#include <utils.h>

ClockDriver *clock_driver;
DmxClockCommandReader *command_reader;
DmxSource *dmx_source;


void setup() { 
    clock_driver = new ClockDriver();
    dmx_source = new DmxSource();
    command_reader = new DmxClockCommandReader();

    if constexpr (RUN_TESTS_ON_STARTUP)
        run_tests();
}

void loop() {
    if (dmx_source->is_updated()) {
        ClockCommand command = command_reader->read_command(dmx_source->data());
        clock_driver->set_command(command);
        dmx_source->reset_updated();
    }
    clock_driver->run_timestep();
}
