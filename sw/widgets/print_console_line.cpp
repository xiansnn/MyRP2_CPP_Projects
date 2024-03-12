#include "print_console_line.h"
#include <string>

W_DisplayControlledValueOnTerminal::W_DisplayControlledValueOnTerminal(
    uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
    Framebuffer_format format, config_framebuffer_text_t txt_cnf) : UI_Widget(id, width, height, anchor_x, anchor_y,
                                                                              format, txt_cnf)
{
}

W_DisplayControlledValueOnTerminal::~W_DisplayControlledValueOnTerminal()
{
}

void W_DisplayControlledValueOnTerminal::draw()
{
    int display_value = slope * active_displayed_object->get_value() + offset;
    switch (active_displayed_object->get_status())
    {
    case ControlledObjectStatus::HAS_FOCUS:
        printf("[%d] has focus\n",active_displayed_object->id);
        break;
    case ControlledObjectStatus::IS_ACTIVE:
        printf("[%d] is active: %2d %*c\n", active_displayed_object->id, active_displayed_object->get_value(), display_value, '\120');
        break;
    default:
        break;
    }
}

void W_DisplayControlledValueOnTerminal::draw_border()
{// TODO draw border
}

UI_ControlledObject *W_DisplayControlledValueOnTerminal::set_active_displayed_object(ControlledValue *val) 
{
    this->active_displayed_object = val;
    slope = (max_line_width - 1.) / (active_displayed_object->get_max_value() - active_displayed_object->get_min_value());
    offset = 1 - slope * active_displayed_object->get_min_value();
    return this->active_displayed_object;
}