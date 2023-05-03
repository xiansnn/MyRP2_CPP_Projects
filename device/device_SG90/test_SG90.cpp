/**
 * @file test_SG90.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "pico/stdlib.h"
#include "hw_PWM/hw_pwm.h"

#define CH1 8
#define STEP_ns 1000
#define PERIOD_us 20000
#define PHASE_CORRECT true

int main(int argc, char const *argv[])
{
    PWM pwm = PWM(8, CH1 + 1, STEP_ns, PERIOD_us, PHASE_CORRECT);

    int pos_min = 0;   // 0°
    int pos_max = 180; // 180°
    int pos_step = 45; // in °
    int pos = pos_min;
    float duty_cycle_min = 0.5 / 20; // => pos_min
    float duty_cycle_max = 2.5 / 20; // => pos_max
    float duty_cycle{};              // duty_cycle_min + pos/180 * (duty_cycle_max-duty_cycle_min)
    bool going_up = true;

    pwm.set_width_nb_of_step(CH1, 1);
    pwm.start(true);
    while (true)
    {

        if (going_up)
        {
            pos += pos_step;
            if (pos >= pos_max)
            {
                pos = pos_max;
                going_up = false;
            }
        }
        else
        {
            pos -= pos_step;
            if (pos <= pos_min)
            {
                pos = pos_min;
                going_up = true;
            }
        }

        duty_cycle = duty_cycle_min + (float)pos / 180 * (duty_cycle_max - duty_cycle_min);
        pwm.set_duty_cycle(CH1 + 1, duty_cycle);
        sleep_ms(1000);
    }

    return 0;
}
