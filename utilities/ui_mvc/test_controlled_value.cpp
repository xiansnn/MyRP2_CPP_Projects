#include "controlled_value.h"
#include <pico/stdio.h>
#include <string>


int main()
{
    stdio_init_all();
    AbstractControlledValue val0 = AbstractControlledValue( 0, 5, 1, true);

    for (size_t i = 0; i < 8; i++)
    {
        // val0.increment();
        val0.process_control_event(ControlEvent::INCREMENT);
        printf("val = %d\n", val0.get_value());
    }
    for (size_t i = 0; i < 12; i++)
    {
        // val0.decrement();
        val0.process_control_event(ControlEvent::DECREMENT);

        printf("val = %d\n", val0.get_value());
    }

    return 0;
}
