#include "include/probe.h"
#include "hardware/gpio.h"

/// @brief
/// @param channel_
Probe::Probe(uint channel_)
{
    this->_gpio = Probe::_channel_map[channel_];
    gpio_init(this->_gpio);
    gpio_set_dir(this->_gpio, GPIO_OUT);
}
/// @brief
void Probe::hi()
{
    gpio_put(this->_gpio, 1);
}
/// @brief
void Probe::lo()
{
    gpio_put(this->_gpio, 0);
}
