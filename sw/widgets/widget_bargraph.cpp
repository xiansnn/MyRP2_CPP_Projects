#include "widget_bargraph.h"

uint8_t W_HBargraph::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

void W_HBargraph::draw()
{
    for (int i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        uint8_t bar_anchor_x = anchor_x + bar_height - BARGRAPH_BIN_SPACING;
        size_t bar_width = frame_width - bar_height;
        draw_bar(i, bar_anchor_x, anchor_y + i * bar_height, bar_width, bar_height, false);
    }
}

void W_HBargraph::draw_bar(uint8_t bin_number, uint8_t bar_anchor_x, uint8_t bar_anchor_y, size_t bar_width, size_t bar_height, bool with_border)
{
    rect(anchor_x, bar_anchor_y, frame_width, bar_height, true, Framebuffer_color::black); // erase the previous bar area
    if (bin_number == value)
    {
        rect(anchor_x, anchor_y + bin_number * bar_height, bar_height, bar_height, false);
    }
    uint8_t px = convert_level_value_to_px(this->displayed_values->values[bin_number]);

    if (with_border)
        rect(bar_anchor_x, bar_anchor_y, frame_width - 2, bar_height);

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
        rect(bar_start, bar_anchor_y + BARGRAPH_BIN_SPACING, 1, bar_height - BARGRAPH_BIN_SPACING, true);
    else
        rect(bar_start, bar_anchor_y + BARGRAPH_BIN_SPACING, bar_end - bar_start, bar_height - BARGRAPH_BIN_SPACING, true);
}

void W_HBargraph::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    status_has_changed = true;
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
        increment_value();
        printf("event INCREMENT: %d\n", value);
        // controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        // controlled_object_under_focus = controlled_objects[value];
        // controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        printf("event DECREMENT: %d\n", value);
        // controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        // controlled_object_under_focus = controlled_objects[value];
        // controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;

    default:
        break;
    }
}

W_HBargraph::W_HBargraph(uint8_t id, UI_DisplayDevice *screen, uint8_t anchor_x, uint8_t anchor_y,
                         BargraphDisplayedObject *displayed_values, config_widget_t cnf_bar,
                         Framebuffer_format format, config_framebuffer_text_t txt_cnf)
    : UI_Widget(id, cnf_bar.width, cnf_bar.height, anchor_x, anchor_y, format, txt_cnf),
      UI_ControlledObject(id, 0, BARGRAPH_BIN_NUMBER - 1)
{
    this->screen_framebuffer = screen;
    this->config = cnf_bar;
    this->displayed_values = displayed_values;

    px_max = frame_width;
    bar_height = std::max(5, frame_height / BARGRAPH_BIN_NUMBER); //less than 5 pixel height is unconfortable
    px_min = bar_height + BARGRAPH_BIN_SPACING; // leaving room for status flag equal to the height of a bar
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

void BargraphDisplayedObject::set_value_clipped(uint8_t index, int new_value)
{
    this->values[index] = std::min(max_value, std::max(min_value, new_value));
    status_has_changed = true;
}
