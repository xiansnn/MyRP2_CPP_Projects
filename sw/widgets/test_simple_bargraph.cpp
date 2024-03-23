#include "widget_simple_bargraph.h"
#include "ssd1306.h"
#include "ui_mvc.h"

#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

#define MIN_BIN_VALUE 0
#define MAX_BIN_VALUE 100

#define BIN_NUMBER 5
// #define BIN_HEIGHT 8
#define BARGRAPH_HEIGHT 50

config_master_i2c_t cfg_i2c{
    .i2c = i2c0,
    .sda_pin = SSD1306_I2C_SDA_GPIO,
    .scl_pin = SSD1306_I2C_SCL_GPIO,
    .baud_rate = I2C_FAST_MODE};

config_SSD1306_t cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

hw_I2C_master master = hw_I2C_master(cfg_i2c);
SSD1306 display_screen = SSD1306(&master, cfg_ssd1306);

config_simple_bargraph_widget_t cnf_bargraph{
    .bargraph_anchor_x = 0,
    .bargraph_anchor_y = 0,
    .bargraph_width = 64,
    .bargraph_height = BARGRAPH_HEIGHT,
    .with_border = true,
    .bargraph_bin_number = BIN_NUMBER,
};
BargraphDisplayedObject values_bargraph = BargraphDisplayedObject(MIN_BIN_VALUE, MAX_BIN_VALUE);
W_SimpleHBargraph w_bargraph = W_SimpleHBargraph(&display_screen, &values_bargraph, cnf_bargraph);

void simulate_values();

int main()
{
    stdio_init_all();
    display_screen.clear_pixel_buffer_and_show_full_screen();
    values_bargraph.values = {0}; // init bargraph
    w_bargraph.set_current_displayed_object(&values_bargraph);

    while (true)
    {
        simulate_values();
        w_bargraph.refresh();
        sleep_ms(100);
    }

    return 0;
}

void simulate_values()
{
    for (int i = 0; i < w_bargraph.bargraph_bin_number; i++)
    {
        values_bargraph.values[i] += i + 1;
        if ((values_bargraph.values[i] >= values_bargraph.max_value) or (values_bargraph.values[i] <= values_bargraph.min_value))
            values_bargraph.values[i] = values_bargraph.min_value;
    }
    values_bargraph.set_refresh_requested_flag(true);
}
