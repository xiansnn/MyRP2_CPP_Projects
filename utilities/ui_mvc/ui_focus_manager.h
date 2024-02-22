#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>

#include "ui_mvc.h"

class UI_FocusManager : public UI_ControlledObject
{
private:
    std::vector<UI_ControlledObject *> controlled_objects;
    UI_Controller* current_controller;
    UI_Widget* current_widget;

public:
    UI_FocusManager();
    ~UI_FocusManager();
    UI_ControlledObject* active_controlled_object;
    UI_ControlledObject* controlled_object_under_focus;

    void add_controlled_object(UI_ControlledObject *cntrl_obj);

    bool active_controlled_object_has_changed;
    void clear_active_controlled_object_change_flag();

    void process_control_event(ControlEvent event)=0;

};

#endif // FOCUS_MANAGER_H
