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
    bool with_label{true};
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
    WAITING,
    HAS_FOCUS,
    IS_ACTIVE
};

class UI_Controller;
class UI_Widget;
class AbstractDisplayDevice;
class AbstractWidget;

// ---- class UI_ControlledObject
class UI_ControlledObject
{
private:
    ControlledObjectStatus status{ControlledObjectStatus::WAITING}; // TODO to move to an new AbstractDisplayedObject class

protected:
    bool wrap;
    int value;
    int min_value;
    int max_value;
    int increment{1};
    bool refresh_requested{true}; // TODO to move to an new AbstractDisplayedObject class

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
    bool is_refresh_requested();                       // TODO to move to an new AbstractDisplayedObject class
    void clear_refresh_requested_flag();               // TODO to move to an new AbstractDisplayedObject class
    void update_status(ControlledObjectStatus status); // TODO to move to an new AbstractDisplayedObject class
    ControlledObjectStatus get_status();               // TODO to move to an new AbstractDisplayedObject class

    virtual void set_value_clipped(int new_value) = 0;
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
    bool refresh_requested(); // TODO move to AbstractWidget
    void refresh_done();      // TODO move to AbstractWidget

    void set_active_displayed_object(UI_ControlledObject *displayed_object); // TODO move to AbstractWidget
    UI_ControlledObject *get_active_displayed_object();                      // TODO move to AbstractWidget
    virtual void draw() = 0;
    virtual void draw_border() = 0;
};

// -----class UI_WidgetManager : public UI_ControlledObject, public UI_Controller
class UI_WidgetManager : public UI_ControlledObject, public UI_Controller
{
protected:
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    bool active_controlled_object_has_changed;
    UI_ControlledObject *controlled_object_under_focus;
    std::list<UI_Widget *> widgets;
    AbstractDisplayDevice *screen_framebuffer;

    UI_WidgetManager(AbstractDisplayDevice *screen = nullptr);
    ~UI_WidgetManager();
    void refresh();
    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    void add_widget(UI_Widget *widget);
    void clear_active_controlled_object_change_flag();
    void process_control_event(ControlEvent event) = 0;
};

// -------class AbstractDisplayDevice : public Framebuffer
class AbstractDisplayDevice : public Framebuffer
{
private:
public:
    AbstractDisplayDevice(size_t width, size_t height, Framebuffer_format format = Framebuffer_format::MONO_VLSB, config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    virtual ~AbstractDisplayDevice();
    virtual void show() = 0;
    virtual void show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y) = 0;
};

class AbstractModelObject
{
private:
    AbstractWidget *displaying_widget;
    ControlledObjectStatus status{ControlledObjectStatus::WAITING};
    bool has_changed_flag{true}; // a flag that trigger the execution of refresh, the flag is set by the requester and clear after refresh is done
public:
    AbstractModelObject();
    ~AbstractModelObject();

    void set_current_widget(AbstractWidget *new_widget);
    void update_status(ControlledObjectStatus new_status);
    ControlledObjectStatus get_status();
    bool has_changed();
    void set_change_flag();
    void clear_change_flag();
};

// -------class AbstractWidget : public Framebuffer
class AbstractWidget : public Framebuffer
{
private:
    AbstractDisplayDevice *display_screen;
    uint8_t widget_anchor_x;
    uint8_t widget_anchor_y;
    bool widget_with_border;

protected:
    uint8_t widget_start_x;
    uint8_t widget_start_y;
    uint8_t widget_width;
    uint8_t widget_height;
    uint8_t widget_border_width;
    AbstractModelObject *displayed_object;
    virtual void draw_border();

public:
    static Framebuffer_color blinking_us(uint32_t blink_period);

    AbstractWidget(AbstractDisplayDevice *_display_screen, size_t _frame_width, size_t _frame_height,
                   uint8_t _widget_anchor_x, uint8_t _widget_anchor_y, bool _widget_with_border, uint8_t _widget_border_width = 1,
                   Framebuffer_format _framebuffer_format = Framebuffer_format::MONO_VLSB, config_framebuffer_text_t _framebuffer_txt_cnf = {.font = font_8x8});
    virtual ~AbstractWidget();

    void set_displayed_object(AbstractModelObject *_displayed_object);

    virtual void refresh();

    virtual void draw() = 0;
};

class AbstractControlledValue : public AbstractModelObject
{
private:

protected:
    int value;
    int min_value;
    int max_value;
    int increment{1};
    bool wrap;

public:
    AbstractControlledValue(int min_value = 0, int max_value = 10, bool wrap = false, int increment = 1);
    ~AbstractControlledValue();

    virtual int get_min_value();
    virtual void set_min_value(int value);
    virtual int get_max_value();
    virtual void set_max_value(int value);
    virtual int get_value();
    virtual void increment_value();
    virtual void decrement_value();

    virtual void set_value_clipped(int new_value);
    virtual void process_control_event(ControlEvent);


};


#endif // UI_MVC_H
