#include "controlled_value.h"

#include <bits/stl_algobase.h>

ControlledValue::ControlledValue(int min_value, int max_value, int increment, bool wrap)
{
    this->max_value = max_value;
    this->min_value = min_value;
    this->increment = increment;
    this->wrap = wrap;
    this->value = std::min(this->max_value, std::max(this->min_value, 0));
}

ControlledValue::~ControlledValue()
{
}

int ControlledValue::get_value()
{
    return value;
}

int ControlledValue::get_min_value()
{
    return min_value;
}

int ControlledValue::get_max_value()
{
    return max_value;
}

void ControlledValue::set_value(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
}

void ControlledValue::increment_value()
{
    value += increment;
    if ((wrap) and (value > max_value))
        value = min_value;
    value = std::min(max_value, std::max(min_value, value));

    has_changed = true;
}

void ControlledValue::decrement_value()
{
    value -= increment;
    if ((wrap) and (value < min_value))
        value = max_value;

    value = std::min(max_value, std::max(min_value, value));
    has_changed = true;
}

void ControlledValue::reset_value()
{
    this->value = std::min(max_value, std::max(min_value, 0));
    has_changed = true;
}

void ControlledValue::clear_change_flag()
{
    has_changed = false;
}
