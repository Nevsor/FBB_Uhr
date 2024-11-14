#include <clock_driver.h>

ClockDriver::ClockDriver()
{
    multi_stepper.addStepper(hour_hand);
    multi_stepper.addStepper(minute_hand);
}

void run_timestep_independent(ClockHandCommand command, AccelStepper stepper) 
{
    switch (command.mode) {
    case MovementMode::DONT_MOVE:
        break;
    case MovementMode::MANUAL:
        stepper.runSpeed();
    case MovementMode::AUTOMATIC:
        stepper.run();
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

void set_stepper_values(ClockHandCommand const& command, AccelStepper &stepper)
{
    float speed = command.speed;
    stepper.setMaxSpeed(speed);

    if (command.direction == Direction::COUNTER_CLOCKWISE)
        speed *= -1;

    if (command.mode == MovementMode::AUTOMATIC) {
        stepper.moveTo(command.position);
    }

    stepper.setSpeed(speed);
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
        minute_hand.setCurrentPosition(0);
        hour_hand.setCurrentPosition(0);
    } 

    set_stepper_values(command.hour_hand, hour_hand);
    set_stepper_values(command.minute_hand, minute_hand);

    if (current_command.synchronization_mode == SynchronizationMode::SYNCHRONOUS) {
        set_stepper_values_synchronous(command);
    }
}