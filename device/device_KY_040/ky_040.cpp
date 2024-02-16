#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

KY040Encoder::KY040Encoder(uint encoder_clk_gpio, uint encoder_dt_gpio, gpio_irq_callback_t call_back,
             config_switch_button_t clk_conf, ControlledValue *ctrl_value)
    : SwitchButtonWithIRQ(encoder_clk_gpio, call_back, clk_conf)
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;
    this->cntrl_value = ctrl_value;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

KY040Encoder::~KY040Encoder()
{
}

void KY040Encoder::interrupt_service_routine(uint32_t current_irq_event_mask)
{
    irq_enabled(false);
    SwitchButtonEvent clk_event = process_IRQ_event(current_irq_event_mask);
    if (clk_event == SwitchButtonEvent::PUSH)
    {
        bool clockwise_rotation = gpio_get(dt_gpio);
        if (clockwise_rotation)
            this->cntrl_value->increment_value();
        else
            this->cntrl_value->decrement_value();
    }
    irq_enabled(true);
}

ControlledValue *KY040Encoder::set_cntrl_value(ControlledValue *val)
{
    this->cntrl_value = val;
    return cntrl_value;
}

ControlledValue *KY040Encoder::get_cntrl_value()
{
    return cntrl_value;
}
