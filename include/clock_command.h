#pragma once

enum class MovementMode {
   MANUAL,
   AUTOMATIC,
   DONT_MOVE,
};

enum class Direction {
    CLOCKWISE,
    COUNTER_CLOCKWISE,
};

struct ClockHandCommand {
    int position;
    int speed;
    Direction direction;
    MovementMode mode;
};

struct ClockCommand {
    ClockHandCommand hour_hand;
    ClockHandCommand minute_hand;
    bool set_zero;
};