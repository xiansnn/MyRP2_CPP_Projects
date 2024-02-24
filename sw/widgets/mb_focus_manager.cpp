#include "mb_focus_manager.h"
#include <string>

MB_FocusManager::MB_FocusManager() : UI_FocusManager()
{
}

MB_FocusManager::~MB_FocusManager()
{
}

void MB_FocusManager::process_control_event(SwitchButton *controller)
{
    ControlEvent sw_event = controller->process_sample_event();
    process_control_event(sw_event);
}

void MB_FocusManager::process_control_event(ControlEvent event)
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
        printf("long push\n");
        active_controlled_object->set_value_clipped(0);
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (active_controlled_object->id == FOCUS_MANAGER_ID)
        {
            active_controlled_object->is_active=false;
            active_controlled_object = controlled_objects[value];
            active_controlled_object->is_active=true;
            // active_controlled_object->is_active = true;
        }
        else
        {
            // active_controlled_object->is_active = false;
            active_controlled_object = this;
        }
        // printf("-focus_mngr-new active_controlled_object[%d]\n", active_controlled_object->id);
        active_controlled_object_has_changed = true;
        break;
    case ControlEvent::INCREMENT:
        value++;
        if (value > max_value)
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        controlled_object_under_focus->has_focus = false;
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->has_focus = true;

        value_has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value--;
        if (value < min_value)
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        controlled_object_under_focus->has_focus = false;
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->has_focus = true;
        value_has_changed = true;
        break;

    default:
        break;
    }
}
