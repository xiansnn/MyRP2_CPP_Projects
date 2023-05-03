#if !defined(HC_SR04_H)
#define HC_SR04_H

#include "pico/stdlib.h"

class HC_SR04
{
private:
    uint trig_pin;
    uint echo_pin;
    void trig();
public:
    HC_SR04(uint trig_pin, uint echo_pin);
    float get_distance();
};


#endif // HC_SR04_H
