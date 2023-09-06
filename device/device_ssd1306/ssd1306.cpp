#include "ssd1306.h"


SSD1306::SSD1306(hw_I2C_master *master, config_SSD1306_t default_config)
{
    this->i2c_master = master;
    this->config = default_config;
    this->init_display();
}

void SSD1306::init_display()
{
    printf("init_display\n");
}
