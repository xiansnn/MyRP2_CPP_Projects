#include "ssd1306.h"
#include <stdio.h>

config_master_i2c_t i2c_cfg{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

config_SSD1306_t ssd_cfg{
    .MUX_RATIO = 64};

int main()
{
    stdio_init_all();
    // create I2C bus hw peripheral and display
    hw_I2C_master master = hw_I2C_master(i2c_cfg);
    SSD1306 display = SSD1306(&master, ssd_cfg);

    printf("hello from SSD1306\n");

    return 0;
}
