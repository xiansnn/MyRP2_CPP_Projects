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
        value += increment;
        if ((wrap) and (value > max_value))
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        value_has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value -= increment;
        if ((wrap) and (value < min_value))
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        value_has_changed = true;
        break;

    default:
        break;
    }
}
