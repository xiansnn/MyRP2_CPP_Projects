#include "rotary_encoder.h"
#include "ssd1306.h"

#include "widget_bar.h"
#include "ui_widget_manager.h"

#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

#define ENCODER_ID 11
#define CENTRAL_SWITCH_ID 12

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
};

class StackedBarManager : public UI_WidgetManager
{
private:
    /* data */
public:
    StackedBarManager(UI_DisplayDevice *screen);
    ~StackedBarManager();

    void process_control_event(SwitchButton *controller);
    void process_control_event(ControlEvent event);
};

hw_I2C_master master = hw_I2C_master(cfg_i2c);
SSD1306 screen = SSD1306(&master, cfg_ssd1306);

#define BARGRAPH_BIN_NUMBER 7 // must be define at compile time

#define CONTROLLED_VAL0_ID 21
#define WIDGET_BAR0_ID 31
#define CONSOLE_WIDGET_ID 40
#define MIN_BIN_VALUE 0
#define MAX_BIN_VALUE 10
#define BARGRAPH_ANCHOR_X 0
#define BARGRAPH_ANCHOR_Y 8
#define BARGRAPH_WIDTH 120
#define BARGRAPH_HEIGHT 56
#define BARGRAPH_BIN_HEIGHT BARGRAPH_HEIGHT / BARGRAPH_BIN_NUMBER // font height if a label is written, otherwise totalBargraph height/BARGRAPH_BIN_NUMBER

config_widget_t cfg_bar{
    .width = BARGRAPH_WIDTH,
    .height = BARGRAPH_BIN_HEIGHT,
    .with_border = false,
    .with_label = true,
    .font = font_5x8};

class W_StackedBar
{
private:
    ControlledValue *values[BARGRAPH_BIN_NUMBER];
    W_Bar *w_bar_values[BARGRAPH_BIN_NUMBER];
    void populate_values();
    void populate_widget_values();
    void link_widget_manager_with_values();
    void link_widget_manager_with_widgets();
    void link_widgets_with_values();
    SwitchButton *validation_switch;
    RotaryEncoder *bin_select_encoder;
    StackedBarManager *widget_manager;

public:
    W_StackedBar(UI_DisplayDevice *screen, SwitchButton *central_switch, RotaryEncoder *encoder);
    ~W_StackedBar();
    void refresh();
    void update_controlled_bin();
};

W_StackedBar w_stacked_bar = W_StackedBar(&screen, &central_switch, &encoder);

int main()

{
    stdio_init_all();
    screen.clear_pixel_buffer_and_show_full_screen();

    while (true)
    {
        w_stacked_bar.refresh();
        w_stacked_bar.update_controlled_bin();
        sleep_ms(20);
    }

    return 0;
}

W_StackedBar::W_StackedBar(UI_DisplayDevice *screen, SwitchButton *central_switch, RotaryEncoder *encoder)
{
    widget_manager = new StackedBarManager(screen);
    this->validation_switch = central_switch;
    this->bin_select_encoder = encoder;
    this->bin_select_encoder->set_active_controlled_object(this->widget_manager);
    // this->bin_select_encoder->set_active_controlled_object(w_stacked_bar.widget_manager->get_active_controlled_object());


    populate_values();
    populate_widget_values();

    link_widget_manager_with_values();
    link_widgets_with_values();
    link_widget_manager_with_widgets();
};
void W_StackedBar::update_controlled_bin()
{
    if (this->widget_manager->active_controlled_object_has_changed) // TODO voir comment supprimer active_controlled_object_has_changed
    {
        encoder.set_active_controlled_object(this->widget_manager->get_active_controlled_object());
        this->widget_manager->clear_active_controlled_object_change_flag();
    }
};
void W_StackedBar::refresh()
{
    this->widget_manager->process_control_event(validation_switch);
    this->widget_manager->refresh();
};

W_StackedBar::~W_StackedBar()
{
    delete widget_manager;
};

void W_StackedBar::populate_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {

        values[i] = new ControlledValue(CONTROLLED_VAL0_ID + i, MIN_BIN_VALUE, MAX_BIN_VALUE);
    }
};

void W_StackedBar::populate_widget_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        w_bar_values[i] = new W_Bar(WIDGET_BAR0_ID + i, values[i],
                                    BARGRAPH_ANCHOR_X, (BARGRAPH_ANCHOR_Y + BARGRAPH_BIN_HEIGHT * i), cfg_bar);
    }
};

void W_StackedBar::link_widget_manager_with_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        widget_manager->add_controlled_object(values[i]);
    }
};
void W_StackedBar::link_widget_manager_with_widgets()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        widget_manager->add_widget(w_bar_values[i]);
    }
};
void W_StackedBar::link_widgets_with_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        w_bar_values[i]->set_active_displayed_object(values[i]);
    }
};

StackedBarManager::StackedBarManager(UI_DisplayDevice *screen) : UI_WidgetManager(screen)
{
}

StackedBarManager::~StackedBarManager()
{
}

void StackedBarManager::process_control_event(SwitchButton *controller)
{
    ControlEvent sw_event = controller->process_sample_event();
    process_control_event(sw_event);
}

void StackedBarManager::process_control_event(ControlEvent event)
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
