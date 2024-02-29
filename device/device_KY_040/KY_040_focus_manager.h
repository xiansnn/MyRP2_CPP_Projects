#if !defined(KY8_40_FOCUS_MANAGER_H)
#define KY8_40_FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"
#include "ui_widget_manager.h"
#include "switch_button.h"

class KY_40_WidgetManager : public UI_WidgetManager
{
private:
    
public:
    KY_40_WidgetManager(UI_DisplayDevice* screen = nullptr);
    ~KY_40_WidgetManager();
    
    void process_control_event(SwitchButton* controller);
    void process_control_event(ControlEvent event);

};

#endif // KY8_40_FOCUS_MANAGER_H
