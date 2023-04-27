/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "dbg_probe/probe.h"
#include "hw_pwm.h"

#define LED_PIN 6 
#define STEP_ns 500
#define PERIOD_us 100
#define PHASE_CORRECT false

Probe pr_irq = Probe(4);
Probe pr_main = Probe(5);

PWM pwm = PWM(LED_PIN,LED_PIN+1,STEP_ns,PERIOD_us,PHASE_CORRECT);

void on_pwm_wrap() {
    pr_irq.hi();
    pwm.clear_irq();
    static bool going_up = true;
    static float duty_cycle = 0.0;
    static float duty_cycle_step = 0.01;
    if (going_up) {
        duty_cycle += duty_cycle_step;
        if (duty_cycle >= 1.0) {
            duty_cycle = 1.0;
            going_up = false;
        }
    } else {
        duty_cycle -= duty_cycle_step;
        if (duty_cycle <= 0.0) {
            duty_cycle = 0.0;
            going_up = true;
        }
    }
    pwm.set_duty_cycle(LED_PIN+1,duty_cycle);
    pr_irq.lo();
}


int main() {
    pwm.set_width_nb_of_step(LED_PIN,1);
    pwm.set_duty_cycle(LED_PIN+1, 0.01);
    pwm.set_irq(on_pwm_wrap);
    pwm.start(true);

    while (true){
        pr_main.hi();
        sleep_us(10);
        pr_main.lo();
        sleep_us(90);
    }
}
