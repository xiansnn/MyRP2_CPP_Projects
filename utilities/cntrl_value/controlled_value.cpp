#include "controlled_value.h"

#include <bits/stl_algobase.h>

ControlledValue::ControlledValue(uint8_t id, int min_value, int max_value, int increment, bool wrap) : UI_ControlledObject(id, min_value,max_value)
{
    
    this->wrap = wrap;
    this->value = std::min(this->max_value, std::max(this->min_value, 0));
}

ControlledValue::~ControlledValue()
{
}

// int ControlledValue::get_value()
// {
//     return value;
// }

// int ControlledValue::get_min_value()
// {
//     return min_value;
// }

// int ControlledValue::get_max_value()
// {
//     return max_value;
// }

// void ControlledValue::set_value(int new_value)
// {
//     this->value = std::min(max_value, std::max(min_value, new_value));
// }

void ControlledValue::increment()
{
    value += increment_val;
    if ((wrap) and (value > max_value))
        value = min_value;
    value = std::min(max_value, std::max(min_value, value));

    has_changed = true;
}

void ControlledValue::decrement()
{
    value -= increment_val;
    if ((wrap) and (value < min_value))
        value = max_value;

    value = std::min(max_value, std::max(min_value, value));
    has_changed = true;
}

void ControlledValue::on_push()
{
}

void ControlledValue::on_long_push()
{
}

void ControlledValue::on_short_release()
{
}

void ControlledValue::on_long_release()
{
}

// void ControlledValue::reset()
// {
//     this->value = std::min(max_value, std::max(min_value, 0));
//     has_changed = true;
// }

