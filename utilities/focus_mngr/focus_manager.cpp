#include "focus_manager.h"

FocusManager::FocusManager() : UI_ControlledObject(FOCUS_MANAGER_ID)
{
    min_value = 1;
    focus_index = 0;
    add_controlled_object(this);
    set_value(0);
}

FocusManager::~FocusManager()
{
}

void FocusManager::add_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
}

UI_ControlledObject *FocusManager::get_active_controlled_object()
{
    return controlled_objects[value];
}

void FocusManager::process_control_event(ControlEvent event)
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
        value++;
        if (value > max_value)
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        focus_index = value;
        has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value--;
        if (value < min_value)
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        focus_index = value;
        has_changed = true;
        break;

    default:
        break;
    }
}

