/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main()
{
    /// \tag::setup_pwm[]

    /* 
    free running period @125 MHz = 8 ns
    PERIOD = 8ns * (TOP+1)*(CSR_PH_CORRECT + 1)*(DIV_INT + (DIV_FRAC/16))
    */
#define TOP 999
#define DIV_INT 125
#define DIV_FRAC 0

    gpio_set_function(6, GPIO_FUNC_PWM);
    gpio_set_function(7, GPIO_FUNC_PWM);
    uint slice_1 = pwm_gpio_to_slice_num(6);
    pwm_set_clkdiv_int_frac(slice_1, DIV_INT, DIV_FRAC);
    pwm_set_clkdiv_mode(slice_1, PWM_DIV_FREE_RUNNING);
    pwm-pwm_set_output_polarity(slice_1,false,false);

    pwm_set_wrap(slice_1, TOP);
    pwm_set_chan_level(slice_1, PWM_CHAN_A, 1);
    pwm_set_chan_level(slice_1, PWM_CHAN_B, 500);

    gpio_set_function(8, GPIO_FUNC_PWM);
    gpio_set_function(9, GPIO_FUNC_PWM);
    uint slice_2 = pwm_gpio_to_slice_num(8);
    pwm_config config2 = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&config2, PWM_DIV_FREE_RUNNING);
    pwm_config_set_clkdiv_int_frac(&config2, DIV_INT, DIV_FRAC);
    pwm_config_set_output_polarity(&config2,true, false);
    pwm_config_set_phase_correct(&config2,true);
    pwm_config_set_wrap(&config2, TOP);
    pwm_init(slice_2, &config2, false);
    pwm_set_chan_level(slice_2, PWM_CHAN_A, 10);
    pwm_set_chan_level(slice_2, PWM_CHAN_B, 500);

    uint8_t mask = 0x1 << slice_1 | 0x1 << slice_2;
    pwm_set_mask_enabled(mask);

}
