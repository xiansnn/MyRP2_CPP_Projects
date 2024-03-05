#if !defined(WIDGET_STACKED_BAR_H)
#define WIDGET_STACKED_BAR_H
#include "rotary_encoder.h"
#include "switch_button.h"
#include "ui_mvc.h"
#include "ui_widget_manager.h"
#include "widget_bar.h"

#include <string>

#define BARGRAPH_BIN_NUMBER 7 // must be define at compile time

#define CONTROLLED_VAL0_ID 21
#define WIDGET_BAR0_ID 31
#define CONSOLE_WIDGET_ID 40
#define MIN_BIN_VALUE 0
#define MAX_BIN_VALUE 10
#define BARGRAPH_ANCHOR_X 0
#define BARGRAPH_ANCHOR_Y 8
#define BARGRAPH_WIDTH 120
#define BARGRAPH_HEIGHT 56
#define BARGRAPH_BIN_HEIGHT BARGRAPH_HEIGHT / BARGRAPH_BIN_NUMBER // font height if a label is written, otherwise totalBargraph height/BARGRAPH_BIN_NUMBER







class StackedBarManager : public UI_WidgetManager
{
private:
    /* data */
public:
    StackedBarManager(UI_DisplayDevice *screen);
    ~StackedBarManager();

    void process_control_event(SwitchButton *controller);
    void process_control_event(ControlEvent event);
};


class W_StackedBar : W_Bar
{
private:
    ControlledValue *values[BARGRAPH_BIN_NUMBER];
    W_Bar *w_bar_values[BARGRAPH_BIN_NUMBER];
    void populate_values();
    void populate_widget_values();
    void link_widget_manager_with_values();
    void link_widget_manager_with_widgets();
    void link_widgets_with_values();
    SwitchButton *validation_switch;
    RotaryEncoder *bin_select_encoder;
    StackedBarManager *widget_manager;

public:
    W_StackedBar(uint8_t id,UI_DisplayDevice *screen, SwitchButton *central_switch, RotaryEncoder *encoder , 
    ControlledValue* cntrl_value, uint8_t anchor_x, uint8_t anchor_y,  config_widget_t bar_config ={});
    ~W_StackedBar();
    void refresh();
    void update_controlled_bin();
};




#endif // WIDGET_STACKED_BAR_H
