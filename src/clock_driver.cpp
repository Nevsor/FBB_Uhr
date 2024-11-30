#include <clock_driver.h>
#include <configuration.h>

ClockDriver::ClockDriver()
    : hour_hand(AccelStepper::DRIVER, MOTOR1_PUL, MOTOR1_DIR)
    , minute_hand(AccelStepper::DRIVER, MOTOR2_PUL, MOTOR2_DIR)
{
    pinMode(MOTORS_ENABLE, OUTPUT);
    digitalWrite(MOTORS_ENABLE, LOW);

    multi_stepper.addStepper(hour_hand);
    multi_stepper.addStepper(minute_hand);
}

void run_timestep_independent(ClockHandCommand const& command, AccelStepper &stepper) 
{
    switch (command.mode) {
    case MovementMode::DONT_MOVE:
        break;
    case MovementMode::MANUAL:
        stepper.runSpeed();
        break;
    case MovementMode::AUTOMATIC:
        if (stepper.distanceToGo() != 0)
            stepper.runSpeed();
        break;
    }
}

void ClockDriver::run_timestep()
{
    switch (current_command.synchronization_mode)
    {
    case SynchronizationMode::INDEPENDENT:
        run_timestep_independent(current_command.hour_hand, hour_hand);
        run_timestep_independent(current_command.minute_hand, minute_hand);
        break;
        
    case SynchronizationMode::SYNCHRONOUS:
    case SynchronizationMode::CLOCK:
        multi_stepper.run();
        break;
    }
}

void set_stepper_speed(
    ClockHandCommand const& command,
    AccelStepper &stepper,
    int microsteps)
{
    int speed = command.speed * microsteps;

    // SynchronizationMode::SYNCHRONOUS
    // SynchronizationMode::CLOCK
    // The MultiStepper class reads the max speed to set the speeds.
    stepper.setMaxSpeed(speed);

    // SynchronisationMode::INDEPENDENT
    stepper.setSpeed(
        command.direction == Direction::CLOCKWISE
        ? speed
        : - speed
    );
}

int calculate_virtual_position(
    ClockHandCommand const& command, 
    AccelStepper &stepper, 
    int microsteps_per_revolution)
{
    int virtual_position = command.position;
    
    if (command.direction == Direction::CLOCKWISE
        && command.position < stepper.currentPosition()) 
    { 
        virtual_position += microsteps_per_revolution;
    }

    if (command.direction == Direction::COUNTER_CLOCKWISE
        && command.position > stepper.currentPosition()) 
    { 
        virtual_position -= microsteps_per_revolution;
    }
    
    return virtual_position;
}

void ClockDriver::set_stepper_values_synchronous(ClockCommand const& command) 
{
    long target_positions[2] {
        command.hour_hand.position,
        command.minute_hand.position 
    };
    multi_stepper.moveTo(target_positions);
}

void ClockDriver::set_command(ClockCommand const& command)
{
    current_command = command;
    
    if (command.set_zero) {
        hour_hand.setCurrentPosition(0);
        minute_hand.setCurrentPosition(0);
    } 

    set_stepper_speed(command.hour_hand, hour_hand, HOUR_HAND_MICROSTEPS);
    set_stepper_speed(command.minute_hand, minute_hand, MINUTE_HAND_MICROSTEPS);

    if (current_command.synchronization_mode == SynchronizationMode::SYNCHRONOUS) {
        set_stepper_values_synchronous(command);
    }
}