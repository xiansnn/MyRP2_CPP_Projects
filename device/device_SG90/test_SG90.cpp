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
#include "sg90.h"

#define SYNC_PIN 6
#define COMMAND_PIN 7
#define POS_MIN -90
#define POS_MAX 90


int main()
{
    SG90 motor = SG90(COMMAND_PIN, SYNC_PIN,POS_MIN,POS_MAX);
    int pos_step = 45; // in °
    int pos = POS_MIN;
    bool going_up = true;

    while (true)
    {

        if (going_up)
        {
            pos += pos_step;
            if (pos >= POS_MAX)
            {
                pos = POS_MAX;
                going_up = false;
            }
        }
        else
        {
            pos -= pos_step;
            if (pos <= POS_MIN)
            {
                pos = POS_MIN;
                going_up = true;
            }
        }

        motor.set_pos(pos);
        sleep_ms(1000);
    }

    return 0;
}
