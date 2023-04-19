/**
 * @file hc_sr04.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hc_sr04.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>

#define TRIG_PIN 21
#define ECHO_PIN 26

/**
 * @brief 
 * 
 */
HC_SR04::HC_SR04(/* args */)
{
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIG_PIN,GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_pull_up(ECHO_PIN);
}

void HC_SR04::trig()
{
    gpio_put(TRIG_PIN,1);
    sleep_us(10);
    gpio_put(TRIG_PIN,0);
}

float HC_SR04::get_distance()
{
    this->trig();
       //wait 
    while (gpio_get(ECHO_PIN) == 0)
    {
    }
    absolute_time_t start = get_absolute_time();
    while (gpio_get(ECHO_PIN) == 1)
    {
    } 
    absolute_time_t end = get_absolute_time();
    int64_t travel_time = absolute_time_diff_us(start,end);
    float range =  (float) travel_time * 0.017 ; // 340m/s give 0.0340 cm/us round-trip -> 0.017 cm/us
return range;
}

