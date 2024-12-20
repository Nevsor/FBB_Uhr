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
    
    pinMode(DMX_LED_PIN, OUTPUT);

    if (RUN_TESTS_ON_STARTUP)
        run_tests(*clock_driver);
}

void loop() {
    // if (dmx_source->is_updated()) {
    ClockCommand command = command_reader->read_command(dmx_source->data());
    clock_driver->set_command(command);
        // dmx_source->reset_updated();
    // }
    clock_driver->run_timestep();
    
    if (dmx_source->is_connected() && (millis()/100)%2) {
        digitalWrite(DMX_LED_PIN, HIGH);
    } else {
        digitalWrite(DMX_LED_PIN, LOW);
    }
}
