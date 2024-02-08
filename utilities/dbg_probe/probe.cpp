/**
 * @file probe.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief A utility that can be used to show code behavior through pulse on gpio.
 * Very useful with a logic analyser.
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "probe.h"
#include "hardware/gpio.h"

/**
 * @brief Construct a new Probe:: Probe object
 *
 * @param channel_ this is the channel number of the logic analyser
 */
Probe::Probe(uint channel_)
{
    this->_gpio = Probe::_channel_map[channel_];
    gpio_init(this->_gpio);
    gpio_set_dir(this->_gpio, GPIO_OUT);
}

/**
 * @brief send a HI level on the corresponding gpio
 *
 */
void Probe::hi()
{
    gpio_put(this->_gpio, 1);
}

/**
 * @brief send a LO level on the corresponding gpio
 *
 */
void Probe::lo()
{
    gpio_put(this->_gpio, 0);
}

void Probe::pulse_us(uint duration)
{
    hi();
    busy_wait_us(duration);
    lo();
}

void Probe::copy(bool value)
{
    gpio_put(this->_gpio, value);
}
