#include "ui_widget_manager.h"
#include "ui_mvc.h"
// #include <string>

UI_WidgetManager::UI_WidgetManager(UI_DisplayDevice *screen) : UI_ControlledObject(FOCUS_MANAGER_ID), UI_Controller(FOCUS_MANAGER_ID)
{
    this->screen_framebuffer = screen;
    min_value = 0;
    set_value_clipped(0);
    active_controlled_object = this;
}

UI_WidgetManager::~UI_WidgetManager()
{
}

void UI_WidgetManager::refresh()
{
    for (UI_Widget *w : widgets)
    {
        if (w->refresh_requested())
        {
            w->draw();
            if (this->screen_framebuffer != nullptr)
            {
            this->screen_framebuffer->show(w, w->anchor_x, w->anchor_y);
            }
            
            w->refresh_done();
        }
    }
};

void UI_WidgetManager::add_controlled_object(UI_ControlledObject *cntrl_obj) //
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
    this->controlled_object_under_focus = cntrl_obj;
}

void UI_WidgetManager::add_widget(UI_Widget *widget)
{
    this->widgets.push_back(widget);
}

void UI_WidgetManager::clear_active_controlled_object_change_flag()
{
    active_controlled_object_has_changed = false;
}
