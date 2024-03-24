#include "controlled_value.h"

#include <bits/stl_algobase.h>

ControlledValue::ControlledValue(uint8_t id, int min_value, int max_value, int increment, bool wrap) : UI_ControlledObject(id, min_value, max_value)
{
    this->wrap = wrap;
    this->value = std::min(this->max_value, std::max(this->min_value, 0));
}

ControlledValue::~ControlledValue()
{
}

void ControlledValue::process_control_event(ControlEvent event)
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
        /* code */
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        /* code */
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        /* code */
        break;
    case ControlEvent::INCREMENT:
        increment_value();
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        break;

    default:
        break;
    }
}

void ControlledValue::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    refresh_requested = true;
}

SingleControlledValue::SingleControlledValue(int min_value, int max_value, bool wrap, int increment)
    : AbstractModelObject()
{
    this->value = 0;
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment = increment;
    this->wrap = wrap;
}

SingleControlledValue::~SingleControlledValue()
{
}

int SingleControlledValue::get_min_value()
{
    return min_value;
}

void SingleControlledValue::set_min_value(int value)
{
    this->min_value = value;
}

int SingleControlledValue::get_max_value()
{
    return max_value;
}

void SingleControlledValue::set_max_value(int value)
{
    this->max_value = value;
}

int SingleControlledValue::get_value()
{
    return value;
}

void SingleControlledValue::increment_value()
{
    value += increment;
    if ((wrap) and (value > max_value))
        value = min_value;
    this->value = std::min(max_value, std::max(min_value, value));
    this->set_change_flag();
}

void SingleControlledValue::decrement_value()
{
    value -= increment;
    if ((wrap) and (value < min_value))
        value = max_value;
    this->value = std::min(max_value, std::max(min_value, value));
    this->set_change_flag();
}

void SingleControlledValue::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    this->set_change_flag();
}

void SingleControlledValue::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::INCREMENT:
        increment_value();
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        break;

    default:
        break;
    }
}
