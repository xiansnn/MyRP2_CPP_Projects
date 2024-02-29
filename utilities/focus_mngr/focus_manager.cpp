#include "focus_manager.h"
#include <string>

KY_40_WidgetManager::KY_40_WidgetManager(UI_DisplayDevice* screen) : UI_WidgetManager( screen)
{
}

KY_40_WidgetManager::~KY_40_WidgetManager()
{
}

void KY_40_WidgetManager::process_control_event(SwitchButton *controller)
{
    ControlEvent sw_event = controller->process_sample_event();
    process_control_event(sw_event);
}

void KY_40_WidgetManager::process_control_event(ControlEvent event)
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
        active_controlled_object->set_value_clipped(0);
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (active_controlled_object->id == FOCUS_MANAGER_ID)
        {
            active_controlled_object->update_status(ControlledObjectStatus::WAIT);
            active_controlled_object = controlled_objects[value];
            active_controlled_object->update_status(ControlledObjectStatus::IS_ACTIVE);
        }
        else
        {
            active_controlled_object = this;
        }
        active_controlled_object_has_changed = true;
        break;
    case ControlEvent::INCREMENT:
        value++;
        if (value > max_value)
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);

        status_has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value--;
        if (value < min_value)
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        status_has_changed = true;
       break;

    default:
        break;
    }
}
