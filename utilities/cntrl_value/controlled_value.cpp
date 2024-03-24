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
