#include <clock_driver.h>
#include <configuration.h>

ClockDriver::ClockDriver()
    : hour_hand(AccelStepper::DRIVER, MOTOR1_PUL, MOTOR1_DIR)
    , minute_hand(AccelStepper::DRIVER, MOTOR2_PUL, MOTOR2_DIR)
{
    pinMode(MOTORS_ENABLE, OUTPUT);
    digitalWrite(MOTORS_ENABLE, LOW);
    
    // hour_hand.setMinPulseWidth(500);
    // minute_hand.setMinPulseWidth(500);

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
    long microsteps)
{
    float speed = (float)command.speed * microsteps;

    // SynchronizationMode::SYNCHRONOUS
    // SynchronizationMode::CLOCK
    // The MultiStepper class reads the max speed to set the speeds.
    stepper.setMaxSpeed(speed);

    // SynchronisationMode::INDEPENDENT
    // Clockwise is negative speed here, because the gearbox inverts the direction.
    stepper.setSpeed(
        command.direction == Direction::CLOCKWISE
        ? - speed
        : speed
    );
}

long logical_position_to_physical(long position, long microsteps_per_revolution) {
    long physical_position = - position * microsteps_per_revolution / NUM_POSITIONS;
    if (physical_position < 0)
        physical_position += microsteps_per_revolution;

    return physical_position;
}

long calculate_distance(
    ClockHandCommand const& command, 
    AccelStepper &stepper, 
    long microsteps_per_revolution)
{
    long current_pos = stepper.currentPosition() % microsteps_per_revolution; 
    if (current_pos < 0)
        current_pos += microsteps_per_revolution;

    long target_pos = logical_position_to_physical(command.position, microsteps_per_revolution);
    long distance = target_pos - current_pos;
    
    if (command.direction == Direction::COUNTER_CLOCKWISE
        && distance < 0) 
    { 
        distance += microsteps_per_revolution;
    }

    if (command.direction == Direction::CLOCKWISE
        && distance > 0)
    { 
        distance -= microsteps_per_revolution;
    }
    
    return distance;
}

long set_stepper_target_pos_independent(
    ClockHandCommand command, 
    AccelStepper &stepper,
    long microsteps_per_revolution) 
{
    stepper.move(calculate_distance(command, stepper, microsteps_per_revolution));
}

void ClockDriver::set_stepper_target_pos_clock(ClockCommand const& command)
{
    long hour_distance = calculate_distance(command.hour_hand, hour_hand, HOUR_HAND_MICROSTEPS_PER_REVOLUTION); 
    long minute_distance = calculate_distance(command.minute_hand, minute_hand, MINUTE_HAND_MICROSTEPS_PER_REVOLUTION); 

    minute_distance += MINUTE_HAND_MICROSTEPS_PER_REVOLUTION * (12 * hour_distance / HOUR_HAND_MICROSTEPS_PER_REVOLUTION);

    long target_positions[2] {
        hour_hand.currentPosition() + hour_distance,
        minute_hand.currentPosition() + minute_distance,
    };

    multi_stepper.moveTo(target_positions);
}

void ClockDriver::set_stepper_target_pos_synchronous(ClockCommand const& command) 
{
    long target_positions[2] {
        hour_hand.currentPosition() + calculate_distance(command.hour_hand, hour_hand, HOUR_HAND_MICROSTEPS_PER_REVOLUTION),
        minute_hand.currentPosition() + calculate_distance(command.minute_hand, minute_hand, MINUTE_HAND_MICROSTEPS_PER_REVOLUTION),
    };
    multi_stepper.moveTo(target_positions);
}

void normalize_stepper_position(AccelStepper &stepper, long microsteps_per_revolution) {
    stepper.setCurrentPosition(stepper.currentPosition() % microsteps_per_revolution);
}

void ClockDriver::set_command(ClockCommand const& command)
{
    current_command = command;
    
    if (command.set_zero) {
        hour_hand.setCurrentPosition(0);
        minute_hand.setCurrentPosition(0);
    } 

    // normalize_stepper_position(hour_hand, HOUR_HAND_MICROSTEPS_PER_REVOLUTION);
    // normalize_stepper_position(minute_hand, MINUTE_HAND_MICROSTEPS_PER_REVOLUTION);

    set_stepper_speed(command.hour_hand, hour_hand, HOUR_HAND_MICROSTEPS);
    set_stepper_speed(command.minute_hand, minute_hand, MINUTE_HAND_MICROSTEPS);

    switch (current_command.synchronization_mode) {
    case SynchronizationMode::INDEPENDENT:
        set_stepper_target_pos_independent(command.hour_hand, hour_hand, HOUR_HAND_MICROSTEPS_PER_REVOLUTION);
        set_stepper_target_pos_independent(command.minute_hand, minute_hand, MINUTE_HAND_MICROSTEPS_PER_REVOLUTION);
        break;

    case SynchronizationMode::SYNCHRONOUS:
        set_stepper_target_pos_synchronous(command);
        break;
        
    case SynchronizationMode::CLOCK:
        set_stepper_target_pos_clock(command);
        break;
    }
}