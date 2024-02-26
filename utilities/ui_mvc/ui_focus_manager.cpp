#include "ui_focus_manager.h"
#include "ui_mvc.h"
#include <string>

UI_FocusManager::UI_FocusManager() : UI_ControlledObject(FOCUS_MANAGER_ID)
{
    min_value = 0;
    set_value_clipped(0);
    active_controlled_object = this;
}

UI_FocusManager::~UI_FocusManager()
{
}

void UI_FocusManager::add_controlled_object(UI_ControlledObject *cntrl_obj) //
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
    this->controlled_object_under_focus = cntrl_obj;
}


void UI_FocusManager::clear_active_controlled_object_change_flag()
{
    active_controlled_object_has_changed = false;
}
