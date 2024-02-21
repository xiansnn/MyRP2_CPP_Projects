#if !defined(UI_MVC_H)
#define UI_MVC_H

#include "pico/stdlib.h"

enum class ControlEvent
{
    NOOP,
    PUSH,
    DOUBLE_PUSH,
    LONG_PUSH,
    RELEASED_AFTER_LONG_TIME,
    RELEASED_AFTER_SHORT_TIME,
    INCREMENT,
    DECREMENT
};

class UI_Controller;
class UI_Widget;

class UI_ControlledObject
{
private:
protected:
    int value;
    int increment_val{1};
    int min_value;
    int max_value;
    UI_Controller *current_controller;
    UI_Widget *current_widget;

public:
    UI_ControlledObject(uint8_t id, int min_value, int max_value, int increment = 1);
    UI_ControlledObject(uint8_t id);
    ~UI_ControlledObject();

    uint8_t id;
    bool has_changed{false};

    void clear_change_flag();
    void set_controller(UI_Controller *controller);
    void set_widget(UI_Widget *widget);
    virtual void reset();
    virtual int get_value();
    virtual int get_min_value();
    virtual int get_max_value();
    virtual void set_value(int new_value);

    virtual UI_ControlledObject *process_control_event(ControlEvent) = 0;
    virtual void on_push() = 0;
    virtual void on_long_push() = 0;
    virtual void on_short_release() = 0;
    virtual void on_long_release() = 0;
    virtual void increment() = 0;
    virtual void decrement() = 0;
};

class UI_Controller
{
private:
    uint8_t id;

protected:
    UI_ControlledObject *active_controlled_object;

public:
    UI_Controller(uint8_t id);
    ~UI_Controller();

    UI_ControlledObject *set_active_controlled_object(UI_ControlledObject *cntrl_obj);
};

class UI_Widget
{
private:
    uint8_t id;

protected:
public:
    UI_ControlledObject *active_displayed_object;
    UI_Widget(uint8_t id);
    ~UI_Widget();

    virtual UI_ControlledObject *set_active_displayed_object(UI_ControlledObject *displayed_object);
    virtual void draw() = 0;
};

#endif // UI_MVC_H
