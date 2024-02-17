#if !defined(UI_MVC_H)
#define UI_MVC_H

#include <map>
#include "pico/stdlib.h"

class UI_ControlledObject;
class UI_Widget;

class UI_Controller
{
private:
    /* data */
public:
    uint8_t id;

    UI_ControlledObject *controlled_object;

    UI_Controller(uint8_t id);
    ~UI_Controller();

    void set_controlled_object(UI_ControlledObject *cntrl_obj);
};

class UI_ControlledObject
{
private:
    /* data */
public:
    uint8_t id;
    bool has_changed{false};
    std::map<uint8_t, UI_Controller *> controllers;
    std::map<uint8_t, UI_Widget *> widgets;

    UI_ControlledObject(uint8_t id);
    ~UI_ControlledObject();
    void clear_change_flag();

    void add_widget(UI_Widget *widget);
    void add_controller(UI_Controller *controller);

    virtual void on_push() = 0;
    virtual void on_long_push() = 0;
    virtual void on_short_release() = 0;
    virtual void on_long_release() = 0;
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void reset() = 0;
};

class UI_Widget
{
private:
    /* data */
public:
    uint8_t id;
    std::map<uint8_t, UI_ControlledObject *> displayed_objects;

    UI_Widget(uint8_t id);
    ~UI_Widget();

    void add_displayed_object(UI_ControlledObject *displayed_object);
};

#endif // UI_MVC_H
