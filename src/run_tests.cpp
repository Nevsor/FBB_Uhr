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
                50, /* position */
                5, /* speed */
                Direction::CLOCKWISE,
                MovementMode::AUTOMATIC
            }, 
            ClockHandCommand {
                0, /* position */
                5, /* speed */
                Direction::COUNTER_CLOCKWISE,
                MovementMode::MANUAL,
            },
            SynchronizationMode::INDEPENDENT,
            false
        },
        3 * SECONDS
    );
    
    command_reader.add_command(
        ClockCommand {
            ClockHandCommand {
                50, /* position */
                5, /* speed */
                Direction::CLOCKWISE,
                MovementMode::DONT_MOVE
            }, 
            ClockHandCommand {
                0, /* position */
                5, /* speed */
                Direction::COUNTER_CLOCKWISE,
                MovementMode::DONT_MOVE,
            },
            SynchronizationMode::INDEPENDENT,
            false
        },
        6 * SECONDS
    );
    
    command_reader.add_command(
        ClockCommand {
            ClockHandCommand {
                0, /* position */
                5, /* speed */
                Direction::COUNTER_CLOCKWISE,
                MovementMode::AUTOMATIC
            }, 
            ClockHandCommand {
                0, /* position */
                5, /* speed */
                Direction::CLOCKWISE,
                MovementMode::AUTOMATIC,
            },
            SynchronizationMode::SYNCHRONOUS,
            false
        },
        9 * SECONDS
    );
    
    while (millis() < 12 * SECONDS) {
        clock_driver.set_command(command_reader.read_command(nullptr));
        clock_driver.run_timestep();
    }
    
}
