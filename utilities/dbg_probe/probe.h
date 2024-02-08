#ifndef PROBE_H
#define PROBE_H

#include "pico/stdlib.h"

class Probe
{
private:
    uint _gpio;
    constexpr static int _channel_map[8] = {6, 7, 8, 9, 16, 17, 21, 26};

public:
    Probe(uint channel_);
    void hi();
    void lo();
    void pulse_us(uint duration);
    void copy(bool value);
};

#endif