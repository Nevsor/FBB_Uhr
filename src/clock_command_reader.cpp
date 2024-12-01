#include <clock_command_reader.h>

#include <assert.h>

#include <Arduino.h>

#include <configuration.h>
#include <clock_command.h>
#include <utils.h>

bool is_in_range(long value, long min, long max) {
    return min <= value && value <= max;
}

MovementMode read_movement_mode(long value) {
    if ( is_in_range(value, 0, 29) )
        return MovementMode::AUTOMATIC;

    if ( is_in_range(value, 60, 89) )
        return MovementMode::AUTOMATIC;
    
    if ( is_in_range(value, 120, 149) )
        return MovementMode::MANUAL;

    if ( is_in_range(value, 180, 209) )
        return MovementMode::MANUAL;
    
    return MovementMode::DONT_MOVE;
}


Direction read_direction(long value) {
    // MovementMode == AUTOMATIC
    if ( is_in_range(value, 0, 29) )
        return Direction::CLOCKWISE;
    if ( is_in_range(value, 60, 89) )
        return Direction::COUNTER_CLOCKWISE;

    // MovementMode == MANUAL
    if ( is_in_range(value, 120, 149) )
        return Direction::CLOCKWISE;
    if ( is_in_range(value, 180, 209) )
        return Direction::COUNTER_CLOCKWISE;
    
    // MovementMode == DONT_MOVE
    return Direction::CLOCKWISE;
}

constexpr float map_speed(const int speed)
{
    return 
        speed == 0
        ? 0
        : REALTIME_SPEED_STEPS_PER_SECOND * pow(
            pow(MAX_SPEED_STEPS_PER_SECOND/REALTIME_SPEED_STEPS_PER_SECOND, 1.0/245),
            speed-10
          );
}

const float map_speed_lookup_table[256] { map_speed(0), map_speed(1), map_speed(2), map_speed(3), map_speed(4), map_speed(5), map_speed(6), map_speed(7), map_speed(8), map_speed(9), map_speed(10), map_speed(11), map_speed(12), map_speed(13), map_speed(14), map_speed(15), map_speed(16), map_speed(17), map_speed(18), map_speed(19), map_speed(20), map_speed(21), map_speed(22), map_speed(23), map_speed(24), map_speed(25), map_speed(26), map_speed(27), map_speed(28), map_speed(29), map_speed(30), map_speed(31), map_speed(32), map_speed(33), map_speed(34), map_speed(35), map_speed(36), map_speed(37), map_speed(38), map_speed(39), map_speed(40), map_speed(41), map_speed(42), map_speed(43), map_speed(44), map_speed(45), map_speed(46), map_speed(47), map_speed(48), map_speed(49), map_speed(50), map_speed(51), map_speed(52), map_speed(53), map_speed(54), map_speed(55), map_speed(56), map_speed(57), map_speed(58), map_speed(59), map_speed(60), map_speed(61), map_speed(62), map_speed(63), map_speed(64), map_speed(65), map_speed(66), map_speed(67), map_speed(68), map_speed(69), map_speed(70), map_speed(71), map_speed(72), map_speed(73), map_speed(74), map_speed(75), map_speed(76), map_speed(77), map_speed(78), map_speed(79), map_speed(80), map_speed(81), map_speed(82), map_speed(83), map_speed(84), map_speed(85), map_speed(86), map_speed(87), map_speed(88), map_speed(89), map_speed(90), map_speed(91), map_speed(92), map_speed(93), map_speed(94), map_speed(95), map_speed(96), map_speed(97), map_speed(98), map_speed(99), map_speed(100), map_speed(101), map_speed(102), map_speed(103), map_speed(104), map_speed(105), map_speed(106), map_speed(107), map_speed(108), map_speed(109), map_speed(110), map_speed(111), map_speed(112), map_speed(113), map_speed(114), map_speed(115), map_speed(116), map_speed(117), map_speed(118), map_speed(119), map_speed(120), map_speed(121), map_speed(122), map_speed(123), map_speed(124), map_speed(125), map_speed(126), map_speed(127), map_speed(128), map_speed(129), map_speed(130), map_speed(131), map_speed(132), map_speed(133), map_speed(134), map_speed(135), map_speed(136), map_speed(137), map_speed(138), map_speed(139), map_speed(140), map_speed(141), map_speed(142), map_speed(143), map_speed(144), map_speed(145), map_speed(146), map_speed(147), map_speed(148), map_speed(149), map_speed(150), map_speed(151), map_speed(152), map_speed(153), map_speed(154), map_speed(155), map_speed(156), map_speed(157), map_speed(158), map_speed(159), map_speed(160), map_speed(161), map_speed(162), map_speed(163), map_speed(164), map_speed(165), map_speed(166), map_speed(167), map_speed(168), map_speed(169), map_speed(170), map_speed(171), map_speed(172), map_speed(173), map_speed(174), map_speed(175), map_speed(176), map_speed(177), map_speed(178), map_speed(179), map_speed(180), map_speed(181), map_speed(182), map_speed(183), map_speed(184), map_speed(185), map_speed(186), map_speed(187), map_speed(188), map_speed(189), map_speed(190), map_speed(191), map_speed(192), map_speed(193), map_speed(194), map_speed(195), map_speed(196), map_speed(197), map_speed(198), map_speed(199), map_speed(200), map_speed(201), map_speed(202), map_speed(203), map_speed(204), map_speed(205), map_speed(206), map_speed(207), map_speed(208), map_speed(209), map_speed(210), map_speed(211), map_speed(212), map_speed(213), map_speed(214), map_speed(215), map_speed(216), map_speed(217), map_speed(218), map_speed(219), map_speed(220), map_speed(221), map_speed(222), map_speed(223), map_speed(224), map_speed(225), map_speed(226), map_speed(227), map_speed(228), map_speed(229), map_speed(230), map_speed(231), map_speed(232), map_speed(233), map_speed(234), map_speed(235), map_speed(236), map_speed(237), map_speed(238), map_speed(239), map_speed(240), map_speed(241), map_speed(242), map_speed(243), map_speed(244), map_speed(245), map_speed(246), map_speed(247), map_speed(248), map_speed(249), map_speed(250), map_speed(251), map_speed(252), map_speed(253), map_speed(254), map_speed(255), };

ClockHandCommand read_clock_hand_command(uint8_t *hand_data) {
    const long POSITION_OFFSET = 0;
    const long SPEED_OFFSET = 1;
    const long MODE_OFFSET = 2;
    
    long mode_and_direction = hand_data[MODE_OFFSET];

    MovementMode mode = read_movement_mode(mode_and_direction);
    Direction direction = read_direction(mode_and_direction);
    
    long position_data = hand_data[POSITION_OFFSET];
    uint8_t speed = hand_data[SPEED_OFFSET];
    
    if (mode == MovementMode::AUTOMATIC)
        if ( !is_in_range(position_data, 10, 10+NUM_POSITIONS-1) )
            mode = MovementMode::DONT_MOVE;

    return {
        position_data - 10,
        map_speed_lookup_table[speed],
        // map_speed(speed),
        direction,
        mode,
    };
    
}

SynchronizationMode read_synchronization_mode(long value) {
    if (is_in_range(value, 0, 49))
        return SynchronizationMode::INDEPENDENT;
    
    if (is_in_range(value, 50, 99))
        return SynchronizationMode::SYNCHRONOUS;

    if (is_in_range(value, 100, 149))
        return SynchronizationMode::CLOCK;
    
    // MOVEMENT_MODE == DONT_MOVE
    return SynchronizationMode::INDEPENDENT;
}

bool read_set_zero(long value) {
    return is_in_range(value, 210, 229);
}

bool last_channel_is_nop(long value) {
    if (is_in_range(value, 150, 255))
        return true;

    return false;
}

ClockCommand DmxClockCommandReader::read_command(uint8_t *dmx_data) const {
    const long HOUR_HAND_OFFSET = 0;
    const long MINUTE_HAND_OFFSET = 3;
    const long HOLDING_SET_ZERO_OFFSET = 6;
    const long SYNCHRONIZATION_MODE_OFFSET = 6;
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
    
    if (is_in_range(dmx_data[HOLDING_SET_ZERO_OFFSET], 150, 255)) {
        command.hour_hand.mode = MovementMode::DONT_MOVE;
        command.minute_hand.mode = MovementMode::DONT_MOVE;
    }
    
    return command;
}
