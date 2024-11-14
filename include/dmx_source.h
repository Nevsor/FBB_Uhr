#pragma once

#include <stdint.h>

#include <TinyDMXSerial.h>

#include <clock_command.h>
#include <configuration.h>


class DmxSource {
public:
    DmxSource(): dmx(buffer, NUM_CLOCK_CHANNELS) {
       dmx.begin(DMXMode::Receiver);
    };
    bool is_updated() { return dmx.dataUpdated(); }; 
    void reset_updated() { dmx.resetUpdated(); };
    uint8_t *data() { return buffer; };

private:
    uint8_t buffer[NUM_CLOCK_CHANNELS];
    TinyDMXSerial dmx;
};

class StubDmxSource {
public:
    bool is_updated() { return updated; }
    void reset_updated() { updated = false; }
    void set_buffer(uint8_t *data) {
        for (int i; i<NUM_CLOCK_CHANNELS; i++)
            buffer[i] = data[i];
        
        updated = true; 
    }

private:
    uint8_t buffer[NUM_CLOCK_CHANNELS];
    bool updated = false;
};