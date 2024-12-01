#pragma once

#include <AccelStepper.h>
#include <MultiStepper.h>

#include <clock_command.h>


class ClockDriver {
public:
    ClockDriver();

    void set_command(ClockCommand const& command);

    
    // Führt Bewegungen anhand des aktuellen Kommandos aus.
    // Diese Methode muss einmal pro Iteration der Hauptschleife aufgerufen werden
    // und führt maximal einen Schritt pro Motor aus.
    void run_timestep();
    
private:
    void set_stepper_target_pos_synchronous(ClockCommand const& command);
    void set_stepper_target_pos_clock(ClockCommand const& command);

    ClockCommand current_command;
    AccelStepper hour_hand;
    AccelStepper minute_hand;
    MultiStepper multi_stepper;
};