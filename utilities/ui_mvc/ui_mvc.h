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






// ---- class UI_ControlledObject
class UI_ControlledObject
{
private:
    ControlledObjectStatus status{ControlledObjectStatus::WAITING};// TODO to move to an new AbstractDisplayedObject class

protected:
    bool wrap;
    int value;
    int min_value;
    int max_value;
    int increment{1};
    bool status_has_changed{true};  // TODO to move to an new AbstractDisplayedObject class

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
    bool has_status_changed(); // TODO to move to an new AbstractDisplayedObject class
    void clear_status_change_flag(); // TODO to move to an new AbstractDisplayedObject class
    void update_status(ControlledObjectStatus status); // TODO to move to an new AbstractDisplayedObject class
    ControlledObjectStatus get_status();  // TODO to move to an new AbstractDisplayedObject class

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
    void refresh_done();// TODO move to AbstractWidget

    void set_active_displayed_object(UI_ControlledObject *displayed_object); // TODO move to AbstractWidget
    UI_ControlledObject *get_active_displayed_object();// TODO move to AbstractWidget
    virtual void draw() = 0;
    virtual void draw_border() = 0;
};

// -----class UI_WidgetManager : public UI_ControlledObject, public UI_Controller
class UI_WidgetManager : public UI_ControlledObject, public UI_Controller
{
protected:
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    UI_WidgetManager(AbstractDisplayDevice *screen = nullptr);
    ~UI_WidgetManager();
    UI_ControlledObject *controlled_object_under_focus;
    std::list<UI_Widget *> widgets;
    AbstractDisplayDevice *screen_framebuffer;
    void refresh();
    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    void add_widget(UI_Widget *widget);
    bool active_controlled_object_has_changed;
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



class AbstractDisplayedObject
{
private:
    /* data */
public:
    AbstractDisplayedObject(/* args */);
    ~AbstractDisplayedObject();
};

class AbstractControlledObject
{
private:
    /* data */
public:
    AbstractControlledObject(/* args */);
    ~AbstractControlledObject();
};





// -------class AbstractWidget : public Framebuffer
class AbstractWidget : public Framebuffer
{
private:
protected:
    AbstractDisplayDevice *display_screen;
    bool widget_with_border;
    uint8_t widget_border_width;
    uint8_t widget_start_x;
    uint8_t widget_start_y;
    uint8_t widget_width;
    uint8_t widget_height;
public:
    AbstractWidget(AbstractDisplayDevice *_display_screen, size_t _frame_width, size_t _frame_height, uint8_t _widget_anchor_x, uint8_t _widget_anchor_y, bool _widget_with_border, uint8_t _widget_border_width = 1,
                   Framebuffer_format _framebuffer_format = Framebuffer_format::MONO_VLSB, config_framebuffer_text_t _framebuffer_txt_cnf = {.font = font_8x8});
    virtual ~AbstractWidget();

    static Framebuffer_color blinking_us(uint32_t blink_period);

    uint8_t widget_anchor_x;
    uint8_t widget_anchor_y;

    virtual void refresh();
    virtual void draw_border();
    virtual void draw() = 0;
};



#endif // UI_MVC_H
