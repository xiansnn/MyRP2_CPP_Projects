#include "ky_040.h"
#include "hardware/gpio.h"

KY040::KY040(uint encoder_clk_gpio, uint encoder_dt_gpio, switch_button_config_t clk_conf)
: SwitchButton(encoder_clk_gpio,clk_conf)
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;
    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
    assert(gpio_get(this->dt_gpio) == GPIO_HI);
}

KY040::~KY040()
{
}

EncoderEvent KY040::get_event()
{
    SwitchButtonEvent clk_event = SwitchButton::get_event();
    if (clk_event != SwitchButtonEvent::PUSHED)
    {
        return EncoderEvent::NOOP;
    }else
    {
        bool dt = gpio_get(dt_gpio);
        return (dt)?EncoderEvent::INCREMENT:EncoderEvent::DECREMENT;
    }
}

KY040_IRQ::KY040_IRQ(uint encoder_dt_gpio, uint encoder_clk_gpio, gpio_irq_callback_t call_back,
                     switch_button_config_t clk_gpio_conf, uint32_t clk_event_mask)
    : KY040(encoder_clk_gpio, encoder_dt_gpio, clk_gpio_conf)
{
}

KY040_IRQ::~KY040_IRQ()
{
}