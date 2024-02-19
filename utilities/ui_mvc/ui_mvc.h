#if !defined(UI_MVC_H)
#define UI_MVC_H

#include <map>
#include "pico/stdlib.h"

class UI_ControlledObject
{
private:
public:
    UI_ControlledObject(uint8_t id);
    ~UI_ControlledObject();

    uint8_t id;
    bool has_changed{false};

    void clear_change_flag();
    virtual void on_push() = 0;
    virtual void on_long_push() = 0;
    virtual void on_short_release() = 0;
    virtual void on_long_release() = 0;
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void reset() = 0;
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

    UI_ControlledObject *set_active_displayed_object(UI_ControlledObject *displayed_object);
};

#endif // UI_MVC_H
