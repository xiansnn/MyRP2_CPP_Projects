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
#include "hw_pwm.h"


#define STEP_ns 2000        // arbitrary value, gives resolution step_ns/(pos_max_us - pos_min_us) = 100ppm.
#define PHASE_CORRECT false // if PHASE_CORRECt = true, channel B is used as a synch pulse in the begining of channel A pulse
// else channel B is used as a synch pulse in the middle of channel A pulse.
#define PERIOD_us 20000 // according to SG90 data sheet.
#define T_MIN_us 500    // according to data sheet, should be 1000us but is 500us actually.
#define T_MAX_us 2500   // according to data sheet, should be 2000us but is 2500us actually.

/**
 * @brief 
 * 
 */
typedef struct config_sg90
{
    uint command_pin;
    uint sync_pin;
    int pos_min_degree = -90;
    int pos_max_degree = +90;
}config_sg90_t;


class SG90
{
private:
    PWM *pwm;
    uint command;
    uint sync;
    int pos_min_deg;
    int pos_max_deg;
    float coef_us_per_degree;

public:
    SG90(config_sg90_t sg90_config);
    void set_pos(int pos);
};

#endif // SG90_H
