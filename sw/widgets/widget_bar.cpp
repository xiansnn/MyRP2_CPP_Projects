#include "widget_bar.h"

#define MAX_LABEL_SIZE 5

uint8_t Bar::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

Bar::Bar(uint8_t id, ControlledValue *cntrl_value, config_bar_widget_t config) : 
Framebuffer(config.width, config.height), UI_Widget(id)
{
    this->config = config;
    this->cntrl_value = cntrl_value;
    if (config.with_label)
        label_value_max_width = MAX_LABEL_SIZE * config.font[FONT_WIDTH];
    else
        label_value_max_width = 0;

    px_max = frame_width;
    px_min = label_value_max_width;
    level_coef = (float)(px_max - px_min) / (cntrl_value->get_max_value() - cntrl_value->get_min_value());
    level_offset = px_max - level_coef * cntrl_value->get_max_value();
}

Bar::~Bar()
{
}

void Bar::draw()
{
    uint8_t px = convert_level_value_to_px(cntrl_value->get_value());
    rect(0, 0, frame_width, frame_height, true, Framebuffer_color::black); // clear the full framebuffer
    if (config.with_label)
        draw_level_value(cntrl_value->get_value());

    if (config.with_border)
        draw_border();

    uint8_t bar_start;
    uint8_t bar_end;
    if (cntrl_value->get_value() >= 0)
    {
        bar_start = convert_level_value_to_px(0);
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = convert_level_value_to_px(0);
    }

    if (cntrl_value->get_value() == 0)
        rect(bar_start, 0, 1, frame_height, true);
    else
        rect(bar_start, 0, bar_end - bar_start, frame_height, true);
}

void Bar::draw_border()
{
    rect(px_min, 0, px_max - px_min, frame_height);
}

void Bar::draw_level_value(int value)
{
    this->clear_text_buffer();
    sprintf(this->text_buffer, "%d", value);
    print_text();
}
