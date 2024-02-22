#include "focus_manager.h"
#include <string>


FocusManager::FocusManager() : UI_ControlledObject(FOCUS_MANAGER_ID)
{
    min_value = 1;
    add_controlled_object(this);
    set_value(0);
    active_controlled_object = this;
}

FocusManager::~FocusManager()
{
}

void FocusManager::add_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
}

UI_ControlledObject *FocusManager::update_active_controlled_object()
{
    active_controlled_object = controlled_objects[value];
    return active_controlled_object;
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
        active_controlled_object->reset();
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        /* code */
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (active_controlled_object->id == FOCUS_MANAGER_ID)
        {
            active_controlled_object = controlled_objects[value];
        }
        else
        {
            active_controlled_object = this;
        }
        printf("-focus_mngr-new active_controlled_object[%d]\n", active_controlled_object->id);
        break;
    case ControlEvent::INCREMENT:
        value++;
        if (value > max_value)
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value--;
        if (value < min_value)
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        has_changed = true;
        break;

    default:
        break;
    }
}
