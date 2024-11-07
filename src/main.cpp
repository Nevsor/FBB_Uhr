#include <Arduino.h>

#include <configuration.h>
#include <dmx.h>
#include <clock_hand_driver.h>

ClockHandDriver hour_hand_driver;
ClockHandDriver minute_hand_driver;

void setup() {
  setup_dmx();
}

void loop() {
  ClockCommand command = read_clock_command();
  
  if (command.set_zero) {
    hour_hand_driver.set_zero();
    minute_hand_driver.set_zero();
  }
  
  hour_hand_driver.set_command(command.hour_hand);
  minute_hand_driver.set_command(command.minute_hand);

}