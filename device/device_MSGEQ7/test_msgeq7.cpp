#include "msgeq7.h"

#define MSGEQ7_STROBE 19
#define MSGEQ7_RESET 18

int main()
{
    stdio_init_all();
    gpio_init(MSGEQ7_STROBE);
    gpio_set_dir(MSGEQ7_STROBE, GPIO_OUT);
    gpio_put(MSGEQ7_STROBE, 0);
    gpio_init(MSGEQ7_RESET);
    gpio_set_dir(MSGEQ7_RESET, GPIO_OUT);
    gpio_put(MSGEQ7_RESET, 0);
    sleep_ms(1);

    while (true)
    {
        gpio_put(MSGEQ7_STROBE, 0);
        gpio_put(MSGEQ7_RESET, 1);
        sleep_us(100);
        gpio_put(MSGEQ7_RESET, 0);
        gpio_put(MSGEQ7_STROBE, 1);
        sleep_us(100);

        for (size_t i = 0; i < 7; i++)
        {
            gpio_put(MSGEQ7_STROBE, 0);
            sleep_us(50);
            gpio_put(MSGEQ7_STROBE, 1);
            sleep_us(50);
        }
        sleep_ms(10);
    }
    return 0;
}
