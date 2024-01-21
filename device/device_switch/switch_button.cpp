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
        assert(gpio_get(this->gpio) == GPIO_HI);
    }
    else
    {
        gpio_pull_down(this->gpio);
        assert(gpio_get(this->gpio) == GPIO_LO);
    }
    this->previous_change_time_us = time_us_64();
    this->previous_button_status = SwitchButtonStatus::RELEASED;
    this->previous_push_state = false;
}

SwitchButton::~SwitchButton()
{
}

SwitchButtonEvent SwitchButton::get_event()
{
    uint64_t delta_us;
    uint64_t current_time_us = time_us_64();
    bool switch_is_pushed = check_switch_pushed();
    if (switch_is_pushed == previous_push_state)
        return SwitchButtonEvent::NOOP;
    else
    {
        delta_us = current_time_us - previous_change_time_us;
        if (delta_us < debounce_delay_us)
            return SwitchButtonEvent::NOOP;
        else
        {
            previous_push_state = switch_is_pushed;
            previous_change_time_us = current_time_us;
            if (switch_is_pushed)
            {
                // printf("%10d pressed. delta_ms:%10d\n", time_us_32(), delta_us / 1000);
                current_status = SwitchButtonStatus::PRESSED;
                return SwitchButtonEvent::PUSH;
            }
            else
            {
                current_status = SwitchButtonStatus::RELEASED;
                return (delta_us < long_press_delay_us) ? SwitchButtonEvent::RELEASE : SwitchButtonEvent::LONG_PUSH;
                /*
            if (delta_us < long_press_delay_us)
            {
                printf("%10d released. delta_ms:%10d\n", time_us_32(), delta_us / 1000);
                return SwitchButtonEvent::RELEASE;
            }
            else
            {
                printf("%10d long push. delta_ms:%10d\n", time_us_32(), delta_us / 1000);
                return SwitchButtonEvent::LONG_PUSH;
            }
                */
            }
        }
    }
}

SwitchButtonStatus SwitchButton::get_status()
{
    return this->current_status;
}

bool SwitchButton::check_switch_pushed()
{
    bool current_gpio_value = gpio_get(this->gpio);
    return (current_gpio_value ^ active_lo ) ? true : false;
}
