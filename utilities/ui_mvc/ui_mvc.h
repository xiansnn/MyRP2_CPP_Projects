#if !defined(UI_MVC_H)
#define UI_MVC_H

#include "pico/stdlib.h"

#define FOCUS_MANAGER_ID 0

enum class ControlEvent
{
    NOOP,
    PUSH,
    DOUBLE_PUSH,
    LONG_PUSH,
    RELEASED_AFTER_LONG_TIME,
    RELEASED_AFTER_SHORT_TIME
};

class UI_ControlledObject
{
protected:
    int value;
    int increment_val{1};
    int min_value;
    int max_value;

public:
    UI_ControlledObject(uint8_t id, int min_value, int max_value, int increment = 1);
    UI_ControlledObject(uint8_t id);
    ~UI_ControlledObject();

    uint8_t id;
    bool has_changed{false};

    void clear_change_flag();
    virtual void reset();
    virtual int get_value();
    virtual int get_min_value();
    virtual int get_max_value();
    virtual void set_value(int new_value);
    
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
public:
    UI_Controller(uint8_t id);
    ~UI_Controller();

    uint8_t id;
    UI_ControlledObject *active_controlled_object;

    UI_ControlledObject *set_active_controlled_object(UI_ControlledObject *cntrl_obj);
};

class UI_Widget
{
private:
public:
    UI_Widget(uint8_t id);
    ~UI_Widget();

    uint8_t id;
    UI_ControlledObject *active_displayed_object;

    virtual UI_ControlledObject *set_active_displayed_object(UI_ControlledObject *displayed_object);
    virtual void draw() = 0;
};

#endif // UI_MVC_H
