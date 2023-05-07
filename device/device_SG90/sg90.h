/**
 * @file sg90.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#if !defined(SG90_H)
#define SG90_H

#include "pico/stdlib.h"
#include "hw_PWM/hw_pwm.h"

class SG90
{
private:
    PWM *pwm;
    uint command;
    uint sync;
    int pos_range_deg;
    int pos_min_deg;
    int pos_max_deg;
    float coef;

public:
    SG90(uint command, uint sync, int pos_min, int pos_max);
    void set_pos(int pos);
};

#endif // SG90_H
