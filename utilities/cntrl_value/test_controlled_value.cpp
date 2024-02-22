#include "controlled_value.h"
#include <pico/stdio.h>
#include <string>

#define CNTRL_VAL_ID 10

int main(int argc, char const *argv[])
{
    stdio_init_all();
    ControlledValue val0 = ControlledValue(CNTRL_VAL_ID, 0, 5, 1, true);

    for (size_t i = 0; i < 8; i++)
    {
        // val0.increment();
        val0.process_control_event(ControlEvent::INCREMENT);
        printf("val[%d] = %d\n", val0.id, val0.get_value());
    }
    for (size_t i = 0; i < 12; i++)
    {
        // val0.decrement();
        val0.process_control_event(ControlEvent::DECREMENT);

        printf("val[%d] = %d\n", val0.id, val0.get_value());
    }

    return 0;
}
