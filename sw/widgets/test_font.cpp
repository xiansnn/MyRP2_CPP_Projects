#include "rotary_encoder.h"
#include "ssd1306.h"

#include "widget_bar.h"
#include "widget_text.h"
#include "print_console_line.h"
#include "ui_widget_manager.h"
#include "ui_mvc.h"
#include "switch_button.h"

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

#define CHARFONT_ID 20

#define FONT_WIDGET_ID 30

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

config_widget_t cfg_bar{
    .width = 128,
    .height = 8,
    .with_border = true,
    .with_label = true,
    .font = font_5x8};

config_switch_button_t cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

RotaryEncoder encoder = RotaryEncoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                      shared_irq_call_back, cfg_encoder_clk);
SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO, cfg_central_switch);

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

class TestFontWidgetManager : public UI_WidgetManager
{
private:
    /* data */
public:
    TestFontWidgetManager(UI_DisplayDevice *screen);
    ~TestFontWidgetManager();

    void process_control_event(SwitchButton *controller);
    void process_control_event(ControlEvent event);
};

class DrawFont : public WText
{
public:
    DrawFont(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
             Framebuffer_format format = Framebuffer_format::MONO_VLSB,
             config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    void draw();
};

ControlledValue char_number = ControlledValue(CHARFONT_ID, 32, 255);

hw_I2C_master master = hw_I2C_master(cfg_i2c);
SSD1306 screen = SSD1306(&master, cfg_ssd1306);

TestFontWidgetManager widget_manager = TestFontWidgetManager(&screen);

DrawFont widget_font = DrawFont(FONT_WIDGET_ID, 125, 8, 0, 16);

int main()
{
    stdio_init_all();
    screen.clear_pixel_buffer_and_show_full_screen();

    encoder.set_active_controlled_object(&widget_manager);

    widget_manager.add_controlled_object(&char_number);

    widget_font.set_active_displayed_object(&char_number);

    widget_manager.add_widget(&widget_font);

    encoder.set_active_controlled_object(widget_manager.get_active_controlled_object());

    while (true)
    {
        pr_D1.hi();
        widget_manager.process_control_event(&central_switch);
        widget_manager.refresh();
        if (widget_manager.active_controlled_object_has_changed) // TODO voir comment supprimer active_controlled_object_has_changed
        {
            pr_D4.hi();
            encoder.set_active_controlled_object(widget_manager.get_active_controlled_object());
            widget_manager.clear_active_controlled_object_change_flag();
            pr_D4.lo();
        }
        pr_D1.lo();

        sleep_ms(100);
    }

    return 0;
}

DrawFont::DrawFont(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
                   Framebuffer_format format,
                   config_framebuffer_text_t txt_cnf) : WText(id, width, height, anchor_x, anchor_y,
                                                              format, txt_cnf)
{
}

void DrawFont::draw()
{

    clear_text_buffer();
    char status;
    switch (active_displayed_object->get_status())
    {
    case ControlledObjectStatus::HAS_FOCUS:
        status = '>';
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        status = '\xB2';
        break;
    default:
        status = ' ';
        break;
    }
    uint8_t i = active_displayed_object->get_value();
    char c = i;
    sprintf(text_buffer, "%c %d  %#X  %c", status, i,i, c );
    print_text();
}

TestFontWidgetManager::TestFontWidgetManager(UI_DisplayDevice *screen) : UI_WidgetManager(screen)
{
    this->wrap = true;
}

TestFontWidgetManager::~TestFontWidgetManager()
{
}

void TestFontWidgetManager::process_control_event(SwitchButton *controller)
{
    ControlEvent sw_event = controller->process_sample_event();
    process_control_event(sw_event);
}

void TestFontWidgetManager::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::NOOP:
        /* code */
        break;
    case ControlEvent::PUSH:
        /* code */
        break;
    case ControlEvent::DOUBLE_PUSH:
        /* code */
        break;
    case ControlEvent::LONG_PUSH:
        active_controlled_object->set_value_clipped(0);
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (active_controlled_object->id == FOCUS_MANAGER_ID)
        {
            active_controlled_object->update_status(ControlledObjectStatus::WAIT);
            active_controlled_object = controlled_objects[value];
            active_controlled_object->update_status(ControlledObjectStatus::IS_ACTIVE);
        }
        else
        {
            active_controlled_object = this;
        }
        active_controlled_object_has_changed = true;
        break;
    case ControlEvent::INCREMENT:
        increment_value();
        controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;

    default:
        break;
    }
}
