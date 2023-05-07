/**
 * @file sg90.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "sg90.h"

#define STEP_ns 2000        // arbitrary value, gives resolution step_ns/(pos_max_us - pos_min_us) = 100ppm
#define PHASE_CORRECT false // if PHASE_CORRECt = true, channel B is used as a synch pulse in the begining of channel A pulse
// else channel B is used as a synch pulse in the middle of channel A pulse
#define PERIOD_us 20000 // according to SG90 data sheet
#define T_MIN_us 500    // according to data sheet, should be 1000us but is 500us actually
#define T_MAX_us 2500   // according to data sheet, should be 2000us but is 2500us actually

/**
 * @brief Construct a new SG90::SG90 object
 *
 * @param command
 * @param sync
 * @param pos_min_deg
 * @param pos_max_deg
 */
SG90::SG90(uint command, uint sync, int pos_min_deg, int pos_max_deg)
{
    this->pwm = new PWM(command, sync, STEP_ns, PERIOD_us, PHASE_CORRECT);
    this->command = command;
    this->pos_min_deg = pos_min_deg;
    this->pos_max_deg = pos_max_deg;
    this->coef = (float)(T_MAX_us - T_MIN_us) / (this->pos_max_deg - this->pos_min_deg); //   coef us/degree

    pwm->set_width_nb_of_step(sync, 1); // set the synch pulse width at 1 step
    pwm->start(true);
}

/**
 * @brief
 *
 * @param pos
 */
void SG90::set_pos(int pos)
{
    int clamp = ((pos>=this->pos_max_deg)?this->pos_max_deg:((pos<=this->pos_min_deg)?this->pos_min_deg:pos));
    float t = this->coef * (clamp - this->pos_min_deg) + T_MIN_us;
    float dutycycle = t / PERIOD_us;
    pwm->set_duty_cycle(this->command, dutycycle);
}
