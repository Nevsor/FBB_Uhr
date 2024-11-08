#include <clock_command.h>

#include <TinyDMXSerial.h>

#include <configuration.h>
#include <clock_command.h>

uint8_t dmx_data[DMX_CHANNELS] = {0};
TinyDMXSerial DMX(dmx_data, DMX_CHANNELS);

void setup_dmx() {
   DMX.begin(DMXMode::Receiver);
}

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

ClockHandCommand read_clock_hand_command(int first_channel) {
    const int POSITION_OFFSET = 0;
    const int SPEED_OFFSET = 1;
    const int MODE_OFFSET = 2;
    
    int mode_and_direction = dmx_data[first_channel + MODE_OFFSET];

    MovementMode mode = read_movement_mode(mode_and_direction);
    Direction direction = read_direction(mode_and_direction);
    
    int position_data = dmx_data[first_channel + POSITION_OFFSET];
    int speed_data = dmx_data[first_channel + SPEED_OFFSET];
    
    if ( !is_in_range(position_data, 11, 190) )
        mode = MovementMode::DONT_MOVE;

    return {
        position_data - 11,
        speed_data,
        direction,
        mode,
    };
    
}

ClockCommand read_clock_command() {
    const int HOUR_HAND_OFFSET = 0;
    const int MINUTE_HAND_OFFSET = 3;
    const int HOLDING_SET_ZERO_OFFSET = 6;
    const int NUM_CLOCK_CHANNELS = 7;
    const unsigned long MAX_MILLIS = ~0ul;
    const int SECONDS = 1000;
    
    static_assert(DMX_FIRST_CLOCK_CHANNEL + NUM_CLOCK_CHANNELS <= DMX_CHANNELS+1);
   
    bool holding_set_zero = 
           dmx_data[HOLDING_SET_ZERO_OFFSET] >= 100 
        && dmx_data[HOLDING_SET_ZERO_OFFSET] <= 200;

    static unsigned long start_holding_set_zero = MAX_MILLIS;
    
    unsigned long now = millis();

    if (!holding_set_zero)
        start_holding_set_zero = now;

    return {
        read_clock_hand_command(DMX_FIRST_CLOCK_CHANNEL + HOUR_HAND_OFFSET),
        read_clock_hand_command(DMX_FIRST_CLOCK_CHANNEL + MINUTE_HAND_OFFSET),
        now - start_holding_set_zero > 5*SECONDS
    };
}

