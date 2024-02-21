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

UI_ControlledObject *ControlledValue::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::INCREMENT:
        value += increment_val;
        if ((wrap) and (value > max_value))
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value -= increment_val;
        if ((wrap) and (value < min_value))
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        has_changed = true;
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        break;

    default:
        break;
    }
    return this;
}

