#include "switch_button.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <string>

SwitchButton::SwitchButton(uint gpio, switch_button_config_t conf)
{
    this->gpio = gpio;
    this->debounce_delay_us = conf.debounce_delay_us;
    this->long_press_delay_us = conf.long_press_delay_us;
    this->active_lo = conf.active_lo;

    gpio_init(this->gpio);
    if (active_lo)
    {
        gpio_pull_up(this->gpio);
        // assert(gpio_get(this->gpio) == GPIO_HI);
    }
    else
    {
        gpio_pull_down(this->gpio);
        // assert(gpio_get(this->gpio) == GPIO_LO);
    }
    this->previous_change_time_us = time_us_64();
    this->previous_button_state = ButtonStatus::INACTIVE;
    this->previous_switch_active_state = false;
}

SwitchButton::~SwitchButton()
{
}

SwitchButtonEvent SwitchButton::get_event()
{
    uint64_t delta_time_us;
    uint64_t current_change_time_us = time_us_64();
    bool switch_is_activated = get_switch_activation_state();
    if (switch_is_activated == previous_switch_active_state)
        return SwitchButtonEvent::NOOP;
    else
    {
        delta_time_us = current_change_time_us - previous_change_time_us;
        if (delta_time_us < debounce_delay_us)
            return SwitchButtonEvent::NOOP;
        else
        {
            previous_switch_active_state = switch_is_activated;
            previous_change_time_us = current_change_time_us;
            if (switch_is_activated)
            {
                current_button_state = ButtonStatus::ACTIVE;
                return SwitchButtonEvent::PUSHED;
            }
            else
            {
                current_button_state = ButtonStatus::INACTIVE;
                return (delta_time_us < long_press_delay_us) ? SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME : SwitchButtonEvent::RELEASED_AFTER_LONG_TIME;
            }
        }
    }
}

ButtonStatus SwitchButton::get_status()
{
    return this->current_button_state;
}

/// @brief 
/// @return 
bool SwitchButton::get_switch_activation_state()
{
    bool gpio_value = gpio_get(this->gpio);
    return ((active_lo && !gpio_value)||(!active_lo && gpio_value ) ) ? true : false;
}

SwitchButtonWithIRQ::SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, switch_button_config_t conf, uint32_t sw_event_mask)
    : SwitchButton(gpio, conf)
{
    gpio_set_irq_enabled_with_callback(gpio, sw_event_mask, true, call_back);
}

SwitchButtonWithIRQ::~SwitchButtonWithIRQ()
{
}
