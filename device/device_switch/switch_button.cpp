#include "switch_button.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <string>

SwitchButton::SwitchButton(uint gpio, switch_button_config_t conf)
{
    this->gpio = gpio;
    this->debounce_delay_us = conf.debounce_delay_us;
    this->long_release_delay_us = conf.long_release_delay_us;
    this->long_push_delay_us = conf.long_push_delay_us;
    this->active_lo = conf.active_lo;

    gpio_init(this->gpio);
    if (active_lo)
    {
        gpio_pull_up(this->gpio);
    }
    else
    {
        gpio_pull_down(this->gpio);
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
    uint64_t time_since_previous_change;
    uint64_t current_time_us = time_us_64();
    bool switch_is_activated = get_switch_activation_state();
    if (switch_is_activated == previous_switch_active_state)
    {
        if (button_logical_state != ButtonStatus::ACTIVE)
        {
            return SwitchButtonEvent::NOOP;
        }
        else
        {
            if (current_time_us - previous_change_time_us >= long_push_delay_us)
            {
                button_logical_state = ButtonStatus::INACTIVE;
                return SwitchButtonEvent::LONG_PUSH;
            }
            else
            {
                return SwitchButtonEvent::NOOP;
            }
        }
    }
    else
    {
        time_since_previous_change = current_time_us - previous_change_time_us;
        if (time_since_previous_change < debounce_delay_us)
            return SwitchButtonEvent::NOOP;
        else
        {
            previous_switch_active_state = switch_is_activated;
            previous_change_time_us = current_time_us;
            if (switch_is_activated)
            {
                current_button_state = ButtonStatus::ACTIVE;
                return SwitchButtonEvent::PUSH;
            }
            else
            {
                current_button_state = ButtonStatus::INACTIVE;
                return (time_since_previous_change < long_release_delay_us) ? SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME : SwitchButtonEvent::RELEASED_AFTER_LONG_TIME;
            }
        }
    }
}

ButtonStatus SwitchButton::get_button_logical_state()
{
    return this->current_button_state;
}

/// @brief
/// @return
bool SwitchButton::get_switch_activation_state()
{
    bool gpio_value = gpio_get(this->gpio);
    return ((active_lo && !gpio_value) || (!active_lo && gpio_value)) ? true : false;
}

SwitchButtonWithIRQ::SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, switch_button_config_t conf, uint32_t sw_event_mask)
    : SwitchButton(gpio, conf)
{
    gpio_set_irq_enabled_with_callback(gpio, sw_event_mask, true, call_back);
}

SwitchButtonWithIRQ::~SwitchButtonWithIRQ()
{
}
