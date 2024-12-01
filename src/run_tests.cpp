#include <run_tests.h>
#include <clock_command_reader.h>
#include <clock_command.h>
#include <clock_driver.h>
#include <utils.h>


void run_tests(ClockDriver &clock_driver) {
    StubClockCommandReader<10> command_reader;
    
    command_reader.add_command(
        ClockCommand {
            ClockHandCommand {
                60, /* position */
                20, /* speed */
                Direction::COUNTER_CLOCKWISE,
                MovementMode::MANUAL,
            }, 
            ClockHandCommand {
                60, /* position */
                20, /* speed */
                Direction::CLOCKWISE,
                MovementMode::MANUAL,
            },
            SynchronizationMode::INDEPENDENT,
            false
        },
        1 * SECONDS
    );
    
    command_reader.add_command(
        ClockCommand {
            ClockHandCommand {
                60, /* position */
                20, /* speed */
                Direction::COUNTER_CLOCKWISE,
                MovementMode::MANUAL,
            }, 
            ClockHandCommand {
                60, /* position */
                20, /* speed */
                Direction::CLOCKWISE,
                MovementMode::MANUAL,
            },
            SynchronizationMode::INDEPENDENT,
            false
        },
        1 * SECONDS
    );
    
    while (millis() < 30 * SECONDS) {
        clock_driver.set_command(command_reader.read_command(nullptr));
        clock_driver.run_timestep();
    }
    
}
