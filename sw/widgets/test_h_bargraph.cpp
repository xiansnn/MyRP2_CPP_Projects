#include "widget_bargraph.h"
#include "rotary_encoder.h"
#include "ssd1306.h"
#include "ui_mvc.h"


#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

#define ENCODER_ID 11
#define CENTRAL_SWITCH_ID 12

#define MIN_BIN_VALUE 0
#define MAX_BIN_VALUE 10
#define BARGRAPH_ANCHOR_X 0
#define BARGRAPH_ANCHOR_Y 0
#define BARGRAPH_WIDTH 120
#define BARGRAPH_HEIGHT 56


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

config_switch_button_t central_switch_conf{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000};

config_widget_t cfg_bar{
    .width = BARGRAPH_WIDTH,
    .height = BARGRAPH_HEIGHT,  
    .with_border = true,
    .with_label = false,
    .font = font_5x8};


void shared_irq_call_back(uint gpio, uint32_t event_mask);

hw_I2C_master master = hw_I2C_master(cfg_i2c);
SSD1306 screen = SSD1306(&master, cfg_ssd1306);

RotaryEncoder encoder = RotaryEncoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                      shared_irq_call_back, cfg_encoder_clk);
SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO, central_switch_conf);

BargraphDisplayedObject bg_values = BargraphDisplayedObject(100);

W_HBargraph w_bargraph = W_HBargraph(10,&screen,BARGRAPH_ANCHOR_X,BARGRAPH_ANCHOR_Y,&bg_values, cfg_bar);

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




int main(int argc, char const *argv[])
{
    stdio_init_all();
    screen.clear_pixel_buffer_and_show_full_screen();
    encoder.set_active_controlled_object(&w_bargraph);
    central_switch.set_active_controlled_object(&w_bargraph);

    while (true)
    {
        ControlEvent event = central_switch.process_sample_event();
        w_bargraph.process_control_event(event);
        w_bargraph.refresh();
        
        sleep_ms(20);
    }

    return 0;
}
