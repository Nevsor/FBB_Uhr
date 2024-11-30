#include <clock_command_reader.h>

#include <assert.h>

#include <Arduino.h>

#include <configuration.h>
#include <clock_command.h>
#include <utils.h>

bool is_in_range(int value, int min, int max) {
    return min <= value && value <= max;
}

MovementMode read_movement_mode(int value) {
    if ( is_in_range(value, 0, 100) )
        return MovementMode::AUTOMATIC;
    
    if ( is_in_range(value, 101, 150) )
        return MovementMode::MANUAL;

    if ( is_in_range(value, 170, 220) )
        return MovementMode::MANUAL;
    
    return MovementMode::DONT_MOVE;
}


Direction read_direction(int value) {
    // MovementMode == AUTOMATIC
    if ( is_in_range(value, 0, 50) )
        return Direction::CLOCKWISE;
    if ( is_in_range(value, 51, 100) )
        return Direction::COUNTER_CLOCKWISE;

    // MovementMode == MANUAL
    if ( is_in_range(value, 101, 150) )
        return Direction::CLOCKWISE;
    if ( is_in_range(value, 170, 220) )
        return Direction::COUNTER_CLOCKWISE;
    
    // MovementMode == DONT_MOVE
    return Direction::CLOCKWISE;
}

ClockHandCommand read_clock_hand_command(uint8_t *hand_data) {
    const int POSITION_OFFSET = 0;
    const int SPEED_OFFSET = 1;
    const int MODE_OFFSET = 2;
    
    int mode_and_direction = hand_data[MODE_OFFSET];

    MovementMode mode = read_movement_mode(mode_and_direction);
    Direction direction = read_direction(mode_and_direction);
    
    int position_data = hand_data[POSITION_OFFSET];
    int speed_data = hand_data[SPEED_OFFSET];
    
    if (mode == MovementMode::AUTOMATIC)
        if ( !is_in_range(position_data, 10, 249) )
            mode = MovementMode::DONT_MOVE;

    return {
        position_data - 10,
        speed_data * MAX_SPEED_MICROSTEPS_PER_SECOND / 255,
        direction,
        mode,
    };
    
}

SynchronizationMode read_synchronization_mode(int value) {
    if (is_in_range(value, 0, 50))
        return SynchronizationMode::INDEPENDENT;
    
    if (is_in_range(value, 51, 100))
        return SynchronizationMode::SYNCHRONOUS;

    if (is_in_range(value, 101, 150))
        return SynchronizationMode::CLOCK;
    
    // MOVEMENT_MODE == DONT_MOVE
    return SynchronizationMode::INDEPENDENT;
}

bool read_set_zero(int value) {
    return is_in_range(value, 175, 200);
}

bool last_channel_is_nop(int value) {
    if (is_in_range(value, 150, 255))
        return true;

    return false;
}

ClockCommand DmxClockCommandReader::read_command(uint8_t *dmx_data) const {
    const int HOUR_HAND_OFFSET = 0;
    const int MINUTE_HAND_OFFSET = 3;
    const int HOLDING_SET_ZERO_OFFSET = 6;
    const int SYNCHRONIZATION_MODE_OFFSET = 6;
    const unsigned long MAX_MILLIS = ~0ul;
   
    static_assert (NUM_CLOCK_CHANNELS == 7);
    static_assert (DMX_FIRST_CLOCK_CHANNEL + NUM_CLOCK_CHANNELS <= DMX_CHANNELS + 1);
    
    ClockCommand command;

    command.hour_hand = read_clock_hand_command(dmx_data + HOUR_HAND_OFFSET);
    command.minute_hand = read_clock_hand_command(dmx_data + MINUTE_HAND_OFFSET);
    command.synchronization_mode = read_synchronization_mode(dmx_data[SYNCHRONIZATION_MODE_OFFSET]);
   
    bool holding_set_zero = read_set_zero(dmx_data[HOLDING_SET_ZERO_OFFSET]);
    static unsigned long start_holding_set_zero = MAX_MILLIS;
    unsigned long now = millis();
    if (!holding_set_zero)
        start_holding_set_zero = now;
    command.set_zero = now - start_holding_set_zero > 5*SECONDS;
    
    if (is_in_range(dmx_data[HOLDING_SET_ZERO_OFFSET], 151, 255)) {
        command.hour_hand.mode = MovementMode::DONT_MOVE;
        command.minute_hand.mode = MovementMode::DONT_MOVE;
    }
    
    return command;
}
