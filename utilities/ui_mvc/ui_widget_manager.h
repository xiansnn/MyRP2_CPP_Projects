#if !defined(UI_FOCUS_MANAGER_H)
#define UI_FOCUS_MANAGER_H

#include <vector>
#include <list>

#include "ui_mvc.h"

class UI_WidgetManager : public UI_ControlledObject, public UI_Controller // TODO a inclure dans ui_mvc
{
protected:
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    UI_WidgetManager(UI_DisplayDevice *screen = nullptr);
    ~UI_WidgetManager();
    UI_ControlledObject *controlled_object_under_focus;
    std::list<UI_Widget *> widgets;
    UI_DisplayDevice *screen_framebuffer;
    void refresh();

    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    void add_widget(UI_Widget *widget);

    bool active_controlled_object_has_changed;
    void clear_active_controlled_object_change_flag();

    void process_control_event(ControlEvent event) = 0;
};

#endif // UI_FOCUS_MANAGER_H
