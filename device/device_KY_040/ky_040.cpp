#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

KY040::KY040(uint encoder_clk_gpio, uint encoder_dt_gpio,
             switch_button_config_t clk_conf)
    : SwitchButton(encoder_clk_gpio, clk_conf)
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

KY040::~KY040()
{
}


EncoderEvent KY040::get_event()
{
    SwitchButtonEvent clk_event = SwitchButton::get_event();
    bool dt = gpio_get(dt_gpio);
    if (clk_event != SwitchButtonEvent::PUSHED)
        return EncoderEvent::NOOP;
    else
        return (dt) ? EncoderEvent::INCREMENT : EncoderEvent::DECREMENT;
}

KY040_IRQ::KY040_IRQ(uint encoder_clk_gpio, uint encoder_dt_gpio,
                     gpio_irq_callback_t call_back, switch_button_config_t clk_conf, uint32_t clk_event_mask)
    : KY040(encoder_clk_gpio, encoder_dt_gpio, clk_conf)
{
    gpio_set_irq_enabled_with_callback(gpio, clk_event_mask, true, call_back);
}

KY040_IRQ::~KY040_IRQ()
{
}
