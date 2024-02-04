#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>
#include "probe.h"

Probe pr_D1 = Probe(1);

KY040_IRQ::KY040_IRQ(uint encoder_clk_gpio, uint encoder_dt_gpio, gpio_irq_callback_t call_back,
                     switch_button_config_t clk_conf, ControlledValue *ctrl_value)
    : SwitchButtonWithIRQ(encoder_clk_gpio, call_back, clk_conf)
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;
    this->cntrl_value = ctrl_value;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

KY040_IRQ::~KY040_IRQ()
{
}

void KY040_IRQ::process_encoder_event()
{
    SwitchButtonEvent clk_event = SwitchButtonWithIRQ::get_event();
    bool clockwise_rotation = gpio_get(dt_gpio);
    if (clk_event == SwitchButtonEvent::PUSH)
    {
        if (clockwise_rotation)
        {
            pr_D1.hi();
            this->cntrl_value->increment_value();
        }
        else
        {
            pr_D1.lo();
            this->cntrl_value->decrement_value();
        }
    }
}

void KY040_IRQ::add_cntrl_value(ControlledValue *val)
{
    this->cntrl_value = val;
}

ControlledValue* KY040_IRQ::get_current_cntrl_value()
{
    return cntrl_value;
}
