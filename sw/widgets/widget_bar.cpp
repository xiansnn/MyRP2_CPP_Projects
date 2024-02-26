#include "widget_bar.h"

#define MAX_LABEL_SIZE 8

uint8_t W_Bar::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

W_Bar::W_Bar(uint8_t id, ControlledValue *cntrl_value, uint8_t anchor_x, uint8_t anchor_y, config_bar_widget_t config) 
                : UI_Widget(id,config.width, config.height, anchor_x, anchor_y)
{
    active_displayed_object = cntrl_value;
    this->config = config;
    if (config.with_label)
        label_value_max_width = MAX_LABEL_SIZE * config.font[FONT_WIDTH];
    else
        label_value_max_width = 0;

    px_max = frame_width;
    px_min = label_value_max_width;
    level_coef = (float)(px_max - px_min) / (active_displayed_object->get_max_value() - active_displayed_object->get_min_value());
    level_offset = px_max - level_coef * active_displayed_object->get_max_value();
}

W_Bar::~W_Bar()
{
}

void W_Bar::draw()
{
    uint8_t px = convert_level_value_to_px(active_displayed_object->get_value());
    rect(0, 0, frame_width, frame_height, true, Framebuffer_color::black); // clear the full framebuffer
    if (config.with_label)
        draw_level_value(active_displayed_object->get_value());

    if (config.with_border)
        draw_border();

    uint8_t bar_start;
    uint8_t bar_end;
    if (active_displayed_object->get_value() >= 0)
    {
        bar_start = convert_level_value_to_px(0);
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = convert_level_value_to_px(0);
    }

    if (active_displayed_object->get_value() == 0)
        rect(bar_start, 0, 1, frame_height, true);
    else
        rect(bar_start, 0, bar_end - bar_start, frame_height, true);
}
void W_Bar::draw_border()
{
    rect(px_min, 0, px_max - px_min, frame_height);
}

void W_Bar::draw_level_value(int value)
{
    this->clear_text_buffer();
    char status;
    switch (active_displayed_object->get_status())
    {
    case ControlledObjectStatus::HAS_FOCUS :
        status = '>';
        break;
    case ControlledObjectStatus::IS_ACTIVE :
        status = '#';
        break;
    default:
        status = ' ';
        break;
    }
    sprintf(this->text_buffer, "%c%+d", status, value);
    print_text();
}
