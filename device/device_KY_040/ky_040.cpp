#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

KY040_IRQ::KY040_IRQ(uint encoder_clk_gpio, uint encoder_dt_gpio,
                     gpio_irq_callback_t call_back, switch_button_config_t clk_conf, uint32_t clk_event_mask)
    : SwitchButton(encoder_clk_gpio, clk_conf)
{

    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);

    gpio_set_irq_enabled_with_callback(gpio, clk_event_mask, true, call_back);
}

KY040_IRQ::~KY040_IRQ()
{
}

EncoderEvent KY040_IRQ::get_event()
{
   SwitchButtonEvent clk_event = SwitchButton::get_event();
    bool dt = gpio_get(dt_gpio);
    if (clk_event != SwitchButtonEvent::PUSH)
        return EncoderEvent::NOOP;
    else
        return (dt) ? EncoderEvent::INCREMENT : EncoderEvent::DECREMENT;
}
