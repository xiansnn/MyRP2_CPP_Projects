/**
 * @file dht11.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DHT11_H
#define DHT11_H

#include "hardware/gpio.h"
#include "pico/stdlib.h"



/**
 * @brief 
 * 
 */
typedef struct
{
    bool is_valid;
    float humidity;
    float temp_celsius;
} dht_reading_t;

/**
 * @brief 
 * 
 */
class DHT11
{
private:
    uint gpio_in;

public:
    void read_from_dht(dht_reading_t *result);
    DHT11(uint gpio_in_);
};

#endif // DHT11_H