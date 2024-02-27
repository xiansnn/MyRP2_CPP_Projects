#include "rotary_encoder.h"
#include "ssd1306.h"

#include "widget_bar.h"
#include "widget_text.h"
#include "print_console_line.h"
#include "mb_focus_manager.h"
#include "ui_mvc.h"

#include <string>
#include <vector>
#include <list>

#include "probe.h"
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

#define ENCODER_ID 11
#define CENTRAL_SWITCH_ID 12

#define CONTROLLED_FMFREQ_ID 20
#define CONTROLLED_VAL1_ID 21
#define CONTROLLED_VAL2_ID 22
#define CONTROLLED_VAL3_ID 23

#define FMFREQ_WIDGET_ID 30
#define BAR1_WIDGET_ID 31
#define BAR2_WIDGET_ID 32
#define BAR3_WIDGET_ID 33

#define CONSOLE_WIDGET_ID 40

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

RotaryEncoder encoder = RotaryEncoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                      shared_irq_call_back, cfg_encoder_clk);
SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO, central_switch_conf);

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
class W_DisplayFocus : public WText
{
public:
    W_DisplayFocus(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
                   Framebuffer_format format = Framebuffer_format::MONO_VLSB,
                   config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    void draw();
};

W_DisplayFocus::W_DisplayFocus(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
                               Framebuffer_format format,
                               config_framebuffer_text_t txt_cnf) : WText(id, width, height, anchor_x, anchor_y,
                                                                          format, txt_cnf)
{
}

void W_DisplayFocus::draw()
{
    clear_text_buffer();
    sprintf(text_buffer, "%d", active_displayed_object->get_value());
    print_text();
}

class W_DrawFMFrequency : public WText
{
public:
    W_DrawFMFrequency(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
                      Framebuffer_format format = Framebuffer_format::MONO_VLSB,
                      config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    void draw();
};

W_DrawFMFrequency::W_DrawFMFrequency(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
                                     Framebuffer_format format,
                                     config_framebuffer_text_t txt_cnf) : WText(id, width, height, anchor_x, anchor_y,
                                                                                format, txt_cnf)
{
}

void W_DrawFMFrequency::draw()
{

    clear_text_buffer();
    char status;
    switch (active_displayed_object->get_status())
    {
    case ControlledObjectStatus::HAS_FOCUS:
        status = '>';
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        status = '#';
        break;
    default:
        status = ' ';
        break;
    }
    sprintf(text_buffer, "%c     %5.1f MHz", status, (float)active_displayed_object->get_value() / 10);
    print_text();
}

ControlledValue fm_freq = ControlledValue(CONTROLLED_FMFREQ_ID, 876, 1079, 1, true);
ControlledValue val1 = ControlledValue(CONTROLLED_VAL1_ID, -10, +10);
ControlledValue val2 = ControlledValue(CONTROLLED_VAL2_ID, 5, 25);
ControlledValue val3 = ControlledValue(CONTROLLED_VAL3_ID, -25, -5);

hw_I2C_master master = hw_I2C_master(cfg_i2c);
SSD1306 screen = SSD1306(&master, cfg_ssd1306);

MB_WidgetManager widget_manager = MB_WidgetManager(&screen);
W_DisplayFocus display_focus = W_DisplayFocus(CONSOLE_WIDGET_ID, 120, 8, 0, 0);

W_DrawFMFrequency widget_fm_frequency = W_DrawFMFrequency(FMFREQ_WIDGET_ID, 120, 8, 0, 16);
W_Bar widget_val1 = W_Bar(BAR1_WIDGET_ID, &val1, 0, 32, cfg_bar);
W_Bar widget_val2 = W_Bar(BAR2_WIDGET_ID, &val2, 0, 40, cfg_bar);
W_Bar widget_val3 = W_Bar(BAR3_WIDGET_ID, &val3, 0, 48, cfg_bar);

int main()
{
    stdio_init_all();
    screen.clear_pixel_buffer_and_show_full_screen();

    encoder.set_active_controlled_object(&widget_manager);

    widget_manager.add_controlled_object(&fm_freq);
    widget_manager.add_controlled_object(&val1);
    widget_manager.add_controlled_object(&val2);
    widget_manager.add_controlled_object(&val3);

    widget_fm_frequency.set_active_displayed_object(&fm_freq);
    widget_val1.set_active_displayed_object(&val1);
    widget_val2.set_active_displayed_object(&val2);
    widget_val3.set_active_displayed_object(&val3);

    widget_manager.add_widget(&widget_fm_frequency);
    widget_manager.add_widget(&widget_val1);
    widget_manager.add_widget(&widget_val2);
    widget_manager.add_widget(&widget_val3);

    encoder.set_active_controlled_object(widget_manager.get_active_controlled_object());

    while (true)
    {
        widget_manager.process_control_event(&central_switch);
        widget_manager.refresh();
        if (widget_manager.active_controlled_object_has_changed) // TODO voir comment supprimer active_controlled_object_has_changed
        {
            pr_D4.hi();
            encoder.set_active_controlled_object(widget_manager.get_active_controlled_object());
            widget_manager.clear_active_controlled_object_change_flag();
            pr_D4.lo();
        }

        sleep_ms(20);
    }

    return 0;
}