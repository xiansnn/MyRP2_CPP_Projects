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
    RELEASED_AFTER_SHORT_TIME,
    INCREMENT,
    DECREMENT
};

class UI_Controller;
class UI_Widget;

class UI_ControlledObject
{
private:
    bool has_focus{false};
    bool is_active{false};
protected:
    int value;
    int increment{1};
    int min_value;
    int max_value;

public:
    UI_ControlledObject(uint8_t id, int min_value=0, int max_value=10, int increment = 1);
    ~UI_ControlledObject();

    uint8_t id;
    bool value_has_changed{true};
    bool status_has_changed{true};

    void set_focus_status(bool value);
    bool get_focus_status();
    void set_active_status(bool value);
    bool get_active_status();
    void clear_status_change_flag();

    void clear_value_change_flag();
    virtual int get_value();
    virtual void set_value_clipped(int new_value);

    virtual int get_min_value();
    virtual void set_min_value(int value);
    virtual int get_max_value();
    virtual void set_max_value(int value);

    virtual void process_control_event(ControlEvent) = 0;
};

class UI_Controller
{
private:
protected:
    UI_ControlledObject *active_controlled_object;

public:
    uint8_t id;
    UI_Controller(uint8_t id);
    ~UI_Controller();

    void set_active_controlled_object(UI_ControlledObject *cntrl_obj);
    UI_ControlledObject *get_active_controlled_object();
    
};

class UI_Widget
{
private:
protected:
    UI_ControlledObject *active_displayed_object;

public:
    uint8_t id;
    UI_Widget(uint8_t id);
    ~UI_Widget();
    bool refresh_requested();
    void refresh_done();


    void set_active_displayed_object(UI_ControlledObject *displayed_object);
    UI_ControlledObject *get_active_displayed_object();
    virtual void draw() = 0;
};

#endif // UI_MVC_H
