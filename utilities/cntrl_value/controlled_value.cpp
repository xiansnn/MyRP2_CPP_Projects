#include "controlled_value.h"

#include <bits/stl_algobase.h>

ControlledValue::ControlledValue(int min_value, int max_value, int increment)
{
    this->max_value = max_value;
    this->min_value = min_value;
    this->increment = increment;
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
    return this->min_value;
}

int ControlledValue::get_max_value()
{
    return this->max_value;
}

void ControlledValue::set_value(int new_value)
{
    this->value = std::min(this->max_value, std::max(this->min_value, new_value));
}

void ControlledValue::increment_value()
{
    value += increment;
    value = std::min(this->max_value, std::max(this->min_value, this->value));
    has_changed = true;
}

void ControlledValue::decrement_value()
{
    value -= increment;
    value = std::min(this->max_value, std::max(this->min_value, this->value));
    has_changed = true;
}

void ControlledValue::reset_value()
{
    this->value = std::min(this->max_value, std::max(this->min_value, 0));
    has_changed = true;
}

void ControlledValue::clear_change_flag()
{
    has_changed = false;
}
