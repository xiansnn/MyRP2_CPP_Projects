#if !defined(UI_MVC_H)
#define UI_MVC_H

#include "pico/stdlib.h"
#include "framebuffer.h"
#include <vector>
#include <list>

#define FOCUS_MANAGER_ID 0

typedef struct config_widget
{
    size_t width{128};
    size_t height{8};
    bool with_border{true};
    bool with_label{false};
    const unsigned char *font{nullptr};
} config_widget_t;

enum class ControlEvent
{
    NOOP,
    PUSH,
    DOUBLE_PUSH, // TODO find a way to do "DOUBLE_PUSH"
    LONG_PUSH,
    RELEASED_AFTER_LONG_TIME,
    RELEASED_AFTER_SHORT_TIME,
    INCREMENT,
    DECREMENT,
    TIME_OUT // TODO find a way to do "TIME_OUT"
};

enum class ControlledObjectStatus
{
    WAIT,
    HAS_FOCUS,
    IS_ACTIVE
};

class UI_Controller;
class UI_Widget;


// ---- class UI_ControlledObject
class UI_ControlledObject
{
private:
    ControlledObjectStatus status{ControlledObjectStatus::WAIT};
protected:
    bool wrap;
    int value;
    int min_value;
    int max_value;
    int increment{1};
    bool status_has_changed{true};
public:
    uint8_t id;
    virtual int get_min_value();
    virtual void set_min_value(int value);
    virtual int get_max_value();
    virtual void set_max_value(int value);
    virtual int get_value();
    virtual void increment_value();
    virtual void decrement_value();

    UI_ControlledObject(uint8_t id, int min_value = 0, int max_value = 10, bool wrap = false, int increment = 1);
    virtual ~UI_ControlledObject();
    bool has_status_changed();
    void clear_status_change_flag();
    void update_status(ControlledObjectStatus status);
    ControlledObjectStatus get_status();

    virtual void set_value_clipped(int new_value)=0;
    virtual void process_control_event(ControlEvent) = 0;
};


// ---- class UI_Controller
class UI_Controller
{
private:
protected:
    UI_ControlledObject *active_controlled_object;
public:
    uint8_t id;
    UI_Controller(uint8_t id);
    virtual ~UI_Controller();

    void set_active_controlled_object(UI_ControlledObject *cntrl_obj);
    UI_ControlledObject *get_active_controlled_object();
};


// ---- class UI_Widget : public Framebuffer
class UI_Widget : public Framebuffer
{
private:
protected:
    UI_ControlledObject *active_displayed_object;
public:
    uint8_t id;
    uint8_t anchor_x;
    uint8_t anchor_y;
    UI_Widget(uint8_t id, size_t width, size_t height, uint8_t anchor_x = 0, uint8_t anchor_y = 0,
              Framebuffer_format format = Framebuffer_format::MONO_VLSB,
              config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    virtual ~UI_Widget();
    bool refresh_requested();
    void refresh_done();

    void set_active_displayed_object(UI_ControlledObject *displayed_object);
    UI_ControlledObject *get_active_displayed_object();
    virtual void draw() = 0;
    virtual void draw_border()=0;
};


// -------class UI_DisplayDevice : public Framebuffer
class UI_DisplayDevice : public Framebuffer
{
private:

public:
    UI_DisplayDevice(size_t width, size_t height, Framebuffer_format format = Framebuffer_format::MONO_VLSB, config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    virtual ~UI_DisplayDevice();
    virtual void show() = 0;
    virtual void show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y) = 0;
};


// -----class UI_WidgetManager : public UI_ControlledObject, public UI_Controller 
class UI_WidgetManager : public UI_ControlledObject, public UI_Controller 
{
protected:
    std::vector<UI_ControlledObject *> controlled_objects;
public:
    UI_WidgetManager(UI_DisplayDevice *screen = nullptr);
    ~UI_WidgetManager();
    UI_ControlledObject *controlled_object_under_focus;
    std::list<UI_Widget *> widgets;
    UI_DisplayDevice *screen_framebuffer;
    void refresh();
    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    void add_widget(UI_Widget *widget);
    bool active_controlled_object_has_changed;
    void clear_active_controlled_object_change_flag();
    void process_control_event(ControlEvent event) = 0;
};

#endif // UI_MVC_H
