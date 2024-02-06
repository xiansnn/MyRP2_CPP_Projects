#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>
#include "probe.h"

/**
 * TODO : fix spurious error of rotation detection. occurs when rise edge is mistaken as falling edge. may be debounce doesn't work fine on rising edge?
 */

Probe pr_D1 = Probe(1);
Probe pr_D2 = Probe(2);

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
    if (clk_event == SwitchButtonEvent::PUSH)
    {
        pr_D1.hi();
        bool clockwise_rotation = gpio_get(dt_gpio);
        if (clockwise_rotation)
        {
            pr_D2.hi();
            this->cntrl_value->increment_value();
        }
        else
        {
            pr_D2.lo();
            this->cntrl_value->decrement_value();
        }
        pr_D1.lo();
    }
}

ControlledValue *KY040::set_cntrl_value(ControlledValue *val)
{
    this->cntrl_value = val;
    return cntrl_value;
}

ControlledValue *KY040::get_cntrl_value()
{
    return cntrl_value;
}
