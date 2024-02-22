#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"

class FocusManager : public UI_ControlledObject
{
private:
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    FocusManager();
    ~FocusManager();
    UI_ControlledObject* active_controlled_object;
    UI_ControlledObject* controlled_object_under_focus;

    void add_controlled_object(UI_ControlledObject *cntrl_obj);

    bool active_controlled_object_has_changed;
    void clear_active_controlled_object_change_flag();
    // UI_ControlledObject *update_active_controlled_object();

    void process_control_event(ControlEvent event);

};

#endif // FOCUS_MANAGER_H
