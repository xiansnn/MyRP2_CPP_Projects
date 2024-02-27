#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"
#include "ui_focus_manager.h"
#include "switch_button.h"

class KY_40_WidgetManager : public UI_WidgetManager
{
private:
    
public:
    KY_40_WidgetManager();
    ~KY_40_WidgetManager();
    
    void process_control_event(SwitchButton* controller);
    void process_control_event(ControlEvent event);

};

#endif // FOCUS_MANAGER_H
