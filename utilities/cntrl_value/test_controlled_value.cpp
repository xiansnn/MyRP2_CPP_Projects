#include "controlled_value.h"
#include <pico/stdio.h>
#include <string>

int main(int argc, char const *argv[])
{
    stdio_init_all();
    ControlledValue val0 = ControlledValue(0, 5, 1, true);

    for (size_t i = 0; i < 8; i++)
    {
        val0.increment();
        printf("val = %d\n",val0.get_value());
    }
    for (size_t i = 0; i < 12; i++)
    {
        val0.decrement();
        printf("val = %d\n",val0.get_value());
    }
    
    return 0;
}
