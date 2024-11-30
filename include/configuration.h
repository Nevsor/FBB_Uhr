#pragma once

constexpr int DMX_CHANNELS = 512;
constexpr int DMX_FIRST_CLOCK_CHANNEL = 506;
constexpr int NUM_CLOCK_CHANNELS = 7;

constexpr bool RUN_TESTS_ON_STARTUP = false;

constexpr int MOTOR1_PUL = 5;
constexpr int MOTOR2_PUL = 6;
constexpr int MOTORS_ENABLE = 8;
constexpr int MOTOR1_DIR = 10;
constexpr int MOTOR2_DIR = 11;

constexpr int DMX_LED_PIN = 5;

constexpr int MAX_SPEED_STEPS_PER_SECOND = 150;
constexpr int HOUR_HAND_MAX_SPEED = MAX_SPEED_STEPS_PER_SECOND * HOUR_HAND_MICROSTEPS;
constexpr int MINUTE_HAND_MAX_SPEED = MAX_SPEED_STEPS_PER_SECOND * MINUTE_HAND_MICROSTEPS;
