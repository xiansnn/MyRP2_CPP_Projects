#include "widget_bargraph.h"
#include "widget_simple_bargraph.h"
#include "rotary_encoder.h"
#include "ssd1306.h"
#include "ui_mvc.h"

#include "probe.h"
Probe pr_D1 = Probe(1);

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

#define ENCODER_ID 0
#define CENTRAL_SWITCH_ID 0

#define MIN_BIN_VALUE 0
#define MAX_BIN_VALUE 100

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

config_switch_button_t cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

hw_I2C_master master = hw_I2C_master(cfg_i2c);
SSD1306 display_screen = SSD1306(&master, cfg_ssd1306);

RotaryEncoder encoder = RotaryEncoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO, shared_irq_call_back, cfg_encoder_clk);
SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO, cfg_central_switch);

config_bargraph_widget_t cnf_bargraph{
    .bargraph_anchor_x = 0,
    .bargraph_anchor_y = 0,
    .bargraph_width = 80,
    .bargraph_height = 35,
    .with_border = false,
    .with_status_flag = true,
    .status_flag_mode = StatusFlagMode::BORDER_FLAG,
    .bargraph_bin_number = 7,
    .with_threshold = true,
};

Bargraph values_bargraph = Bargraph(MIN_BIN_VALUE, MAX_BIN_VALUE);
W_HBargraph w_bargraph = W_HBargraph(&display_screen, &values_bargraph, cnf_bargraph);

config_simple_bargraph_widget_t cnf_selected_bin{
    .bargraph_anchor_x = 0,
    .bargraph_anchor_y = 48,
    .bargraph_width = 128,
    .bargraph_height = 10,
    .with_border = true,
    .bargraph_bin_number = 1,
};
Bargraph selected_bin = Bargraph(MIN_BIN_VALUE, MAX_BIN_VALUE);
W_SimpleHBargraph w_selected_bin = W_SimpleHBargraph(&display_screen, &selected_bin, cnf_selected_bin);

void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        encoder.interrupt_service_routine(event_mask);
        break;

    default:
        printf("unknown IRQ\n");
        break;
    };
};

void simulate_values();
void update_selected_value();

int main()
{
    stdio_init_all();
    display_screen.clear_pixel_buffer_and_show_full_screen();
    encoder.set_active_controlled_object(&w_bargraph);
    central_switch.set_active_controlled_object(&w_bargraph);
    values_bargraph.values = {10, 20, 30, 40, 50, 60, 70}; // init bargraph
    values_bargraph.set_current_widget(&w_bargraph);

    selected_bin.set_current_widget(&w_selected_bin);
    selected_bin.values = {0};

    while (true)
    {
        simulate_values();
        ControlEvent event = central_switch.process_sample_event();
        w_bargraph.process_control_event(event);
        update_selected_value();
        pr_D1.hi();
        w_bargraph.refresh();
        w_selected_bin.refresh();
        pr_D1.lo();
        sleep_ms(100);
    }

    return 0;
}

void simulate_values()
{
    for (size_t i = 0; i < cnf_bargraph.bargraph_bin_number; i++)
    {
        values_bargraph.values[i] += (1 + i);
        if (values_bargraph.values[i] >= values_bargraph.max_value)
            values_bargraph.values[i] = values_bargraph.min_value;
    }
    values_bargraph.set_change_flag();
}

void update_selected_value()
{
    selected_bin.values[0] = values_bargraph.values[w_bargraph.current_active_index];
    selected_bin.set_change_flag();
}
