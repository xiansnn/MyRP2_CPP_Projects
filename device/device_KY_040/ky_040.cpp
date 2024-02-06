#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>
#include "probe.h"

Probe pr_D1 = Probe(1);

KY040::KY040(uint encoder_clk_gpio, uint encoder_dt_gpio, gpio_irq_callback_t call_back,
                     switch_button_config_t clk_conf, ControlledValue *ctrl_value)
    : SwitchButtonWithIRQ(encoder_clk_gpio, call_back, clk_conf)
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;
    this->cntrl_value = ctrl_value;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

KY040::~KY040()
{
}

void KY040::interrupt_service_routine()
{
    SwitchButtonEvent clk_event = get_IRQ_event();
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

ControlledValue* KY040::set_cntrl_value(ControlledValue* val)
{
    this->cntrl_value = val;
    return cntrl_value;
}

ControlledValue* KY040::get_cntrl_value()
{
    return cntrl_value;
}
