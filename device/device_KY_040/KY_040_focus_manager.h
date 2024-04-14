#if !defined(KY8_40_FOCUS_MANAGER_H)
#define KY8_40_FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"
#include "switch_button.h"

class KY_40_WidgetManager : public UI_WidgetManager
{
private:
public:
    KY_40_WidgetManager(AbstractDisplayDevice *screen = nullptr);
    ~KY_40_WidgetManager();

    void process_control_event(SwitchButton *controller);
    void process_control_event(ControlEvent event);
    void set_value_clipped(int new_value);
};

#endif // KY8_40_FOCUS_MANAGER_H
