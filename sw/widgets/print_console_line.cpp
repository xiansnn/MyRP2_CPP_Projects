#include "print_console_line.h"
#include <string>

DisplayEncoderOnTerminal::DisplayEncoderOnTerminal(uint8_t id) : UI_Widget(id)
{
}

DisplayEncoderOnTerminal::~DisplayEncoderOnTerminal()
{
}

void DisplayEncoderOnTerminal::display()
{
    int display_value = slope * active_controlled_value->get_value() + offset;
    printf("LOOP[%d]: %2d %*c\n", active_controlled_value->id, active_controlled_value->get_value(), display_value, '|');
}

void DisplayEncoderOnTerminal::set_active_controlled_value(ControlledValue *val)
{
    active_controlled_value = val;
    slope = (max_line_width - 1.) / (active_controlled_value->get_max_value() - active_controlled_value->get_min_value());
    offset = 1 - slope * active_controlled_value->get_min_value();
}