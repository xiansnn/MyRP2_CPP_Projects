#include "widget_bargraph.h"

uint8_t W_HBargraph::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

void W_HBargraph::draw()
{

    rect(0, 0, frame_width, frame_height, true, Framebuffer_color::black); // erase the whole widget area

    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        draw_bar(i, anchor_x, anchor_y + i * frame_height / BARGRAPH_BIN_NUMBER, frame_width, frame_height / BARGRAPH_BIN_NUMBER, false);
    }
}

void W_HBargraph::draw_bar(uint8_t bin_number, uint8_t x, uint8_t y, size_t w, size_t h, bool with_border)
{
    uint8_t px = convert_level_value_to_px(this->displayed_values->values[bin_number]);
    rect(x, y, w, h, true, Framebuffer_color::black); // clear the full framebuffer

    if (with_border)
        rect(x, y, frame_width, h);

    uint8_t bar_start;
    uint8_t bar_end;
    if (this->displayed_values->values[bin_number] >= 0)
    {
        bar_start = convert_level_value_to_px(0);
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = convert_level_value_to_px(0);
    }

    if (this->displayed_values->values[bin_number] == 0)
        rect(bar_start, y, 1, h, true);
    else
        rect(bar_start, y, bar_end - bar_start, h, true);
}

void W_HBargraph::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::NOOP:
        /* code */
        break;
    case ControlEvent::PUSH:
        printf("event PUSH: %d\n", event);
        break;
    case ControlEvent::DOUBLE_PUSH:
        /* code */
        break;
    case ControlEvent::LONG_PUSH:
        this->set_value_clipped(0);
        printf("event LONG_PUSH: %d\n", event);
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        printf("event RELEASED_AFTER_LONG_TIME: %d\n", event);
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        printf("event RELEASED_AFTER_SHORT_TIME: %d\n", event);
        // if (active_controlled_object->id == FOCUS_MANAGER_ID)
        // {
        //     active_controlled_object->update_status(ControlledObjectStatus::WAIT);
        //     active_controlled_object = controlled_objects[value];
        //     active_controlled_object->update_status(ControlledObjectStatus::IS_ACTIVE);
        // }
        // else
        // {
        //     active_controlled_object = this;
        // }
        // active_controlled_object_has_changed = true;
        break;
    case ControlEvent::INCREMENT:
        printf("event INCREMENT: %d\n", event);
        increment_value();
        // controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        // controlled_object_under_focus = controlled_objects[value];
        // controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;
    case ControlEvent::DECREMENT:
        printf("event DECREMENT: %d\n", event);
        decrement_value();
        // controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        // controlled_object_under_focus = controlled_objects[value];
        // controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;

    default:
        break;
    }
}
#define MAX_LABEL_SIZE 8
W_HBargraph::W_HBargraph(uint8_t id, UI_DisplayDevice *screen, uint8_t anchor_x, uint8_t anchor_y,
                         BargraphDisplayedObject *displayed_values, config_widget_t cnf_bar,
                         Framebuffer_format format, config_framebuffer_text_t txt_cnf)
    : UI_Widget(id, cnf_bar.width, cnf_bar.height, anchor_x, anchor_y, format, txt_cnf),
      UI_ControlledObject(id, min_value, max_value, wrap, increment)
{
    this->screen_framebuffer = screen;
    this->config = cnf_bar;
    this->displayed_values = displayed_values;

    if (config.with_label)
        label_value_max_width = MAX_LABEL_SIZE * config.font[FONT_WIDTH];
    else
        label_value_max_width = 0;

    px_max = frame_width;
    px_min = label_value_max_width;
    level_coef = (float)(px_max - px_min) / (displayed_values->max_value - displayed_values->min_value);
    level_offset = px_max - level_coef * displayed_values->max_value;
}

W_HBargraph::~W_HBargraph()
{
}

void W_HBargraph::refresh()
{
    draw();
    this->screen_framebuffer->show(this, this->anchor_x, this->anchor_y);
}

//---------------------------------------------------------------------

BargraphDisplayedObject::BargraphDisplayedObject(uint8_t id, int min_value, int max_value)
{
    values = {7, 1, 6, 2, 5, 3, 4};
    this->min_value = min_value;
    this->max_value = max_value;
}

BargraphDisplayedObject::~BargraphDisplayedObject()
{
}
