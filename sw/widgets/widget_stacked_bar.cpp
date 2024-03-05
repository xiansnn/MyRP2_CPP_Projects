#include "widget_stacked_bar.h"
#include "widget_bar.h"


W_StackedBar::W_StackedBar(uint8_t id,UI_DisplayDevice *screen, SwitchButton *central_switch, RotaryEncoder *encoder , 
            ControlledValue* cntrl_value, uint8_t anchor_x, uint8_t anchor_y,  config_widget_t bar_config) 
            : W_Bar( id, cntrl_value, anchor_x, anchor_y,  bar_config)
{
    widget_manager = new StackedBarManager(screen);
    this->validation_switch = central_switch;
    this->bin_select_encoder = encoder;
    this->bin_select_encoder->set_active_controlled_object(this->widget_manager);

    populate_values();
    populate_widget_values();

    link_widget_manager_with_values();
    link_widgets_with_values();
    link_widget_manager_with_widgets();
};
void W_StackedBar::update_controlled_bin()
{
    if (this->widget_manager->active_controlled_object_has_changed) // TODO voir comment supprimer active_controlled_object_has_changed
    {
        bin_select_encoder->set_active_controlled_object(this->widget_manager->get_active_controlled_object());
        this->widget_manager->clear_active_controlled_object_change_flag();
    }
};
void W_StackedBar::refresh()
{
    this->widget_manager->process_control_event(validation_switch);
    this->widget_manager->refresh();
};

W_StackedBar::~W_StackedBar()
{
    delete widget_manager;
};

void W_StackedBar::populate_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {

        values[i] = new ControlledValue(CONTROLLED_VAL0_ID + i, MIN_BIN_VALUE, MAX_BIN_VALUE);
    }
};

void W_StackedBar::populate_widget_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        w_bar_values[i] = new W_Bar(WIDGET_BAR0_ID + i, values[i],
                                    BARGRAPH_ANCHOR_X, (BARGRAPH_ANCHOR_Y + BARGRAPH_BIN_HEIGHT * i), this->config);
    }
};

void W_StackedBar::link_widget_manager_with_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        widget_manager->add_controlled_object(values[i]);
    }
};
void W_StackedBar::link_widget_manager_with_widgets()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        widget_manager->add_widget(w_bar_values[i]);
    }
};
void W_StackedBar::link_widgets_with_values()
{
    for (size_t i = 0; i < BARGRAPH_BIN_NUMBER; i++)
    {
        w_bar_values[i]->set_active_displayed_object(values[i]);
    }
};

StackedBarManager::StackedBarManager(UI_DisplayDevice *screen) : UI_WidgetManager(screen)
{
}

StackedBarManager::~StackedBarManager()
{
}

void StackedBarManager::process_control_event(SwitchButton *controller)
{
    ControlEvent sw_event = controller->process_sample_event();
    process_control_event(sw_event);
}

void StackedBarManager::process_control_event(ControlEvent event)
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
        increment_value();
        controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        controlled_object_under_focus->update_status(ControlledObjectStatus::WAIT);
        controlled_object_under_focus = controlled_objects[value];
        controlled_object_under_focus->update_status(ControlledObjectStatus::HAS_FOCUS);
        break;

    default:
        break;
    }
}
