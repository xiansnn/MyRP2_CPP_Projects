#include "ky_040.h"
#include "ssd1306.h"
#include "widget_bar.h"

#include "focus_manager.h"
#include "print_console_line.h"



#include <string>
#include <vector>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

#define ENCODER_ID 11
#define CENTRAL_SWITCH_ID 12

#define CONTROLLED_VAL1_ID 21
#define CONTROLLED_VAL2_ID 22
#define CONTROLLED_VAL3_ID 23

#define CONSOLE_WIDGET_ID 31



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


config_bar_widget_t cfg_bar{
    .width = 128,
    .height = 8,
    .with_border = true,
    .with_label = true,
    .font = font_5x8};

config_switch_button_t central_switch_conf{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000};
void shared_irq_call_back(uint gpio, uint32_t event_mask);
KY040Encoder encoder = KY040Encoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                    shared_irq_call_back, cfg_encoder_clk);

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
}

FocusManager focus_manager = FocusManager();
DisplayControlledValueOnTerminal console = DisplayControlledValueOnTerminal(CONSOLE_WIDGET_ID);



int main()
{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    display.clear_pixel_buffer_and_show_full_screen();
    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO, central_switch_conf);

    encoder.set_active_controlled_object(&focus_manager);
    console.set_active_displayed_object(&focus_manager);
    focus_manager.set_current_controller(&encoder);
    focus_manager.set_current_widget(&console);

    ControlledValue val1 = ControlledValue(CONTROLLED_VAL1_ID, -10, +10);
    focus_manager.add_controlled_object(&val1);
    val1.set_current_controller(&encoder);
    val1.set_current_widget(&console);

    ControlledValue val2 = ControlledValue(CONTROLLED_VAL2_ID, 5, 25);
    focus_manager.add_controlled_object(&val2);
    val2.set_current_controller(&encoder);
    val2.set_current_widget(&console);

    ControlledValue val3 = ControlledValue(CONTROLLED_VAL3_ID, -25, -5);
    focus_manager.add_controlled_object(&val3);

    UI_ControlledObject *current_cntrl_obj = &focus_manager;
    encoder.set_active_controlled_object(current_cntrl_obj);

    console.set_active_displayed_object(current_cntrl_obj);

    while (true)
    {
        if (focus_manager.active_controlled_object->value_has_changed)
        {
            console.draw();
            focus_manager.active_controlled_object->clear_value_change_flag();
        }
        focus_manager.process_control_event(&central_switch);
        if (focus_manager.active_controlled_object_has_changed)
        {
            encoder.set_active_controlled_object(focus_manager.active_controlled_object);
            console.set_active_displayed_object(focus_manager.active_controlled_object);
            console.draw();
            focus_manager.clear_active_controlled_object_change_flag();
        }

        sleep_ms(20);
    }


    
    return 0;
}