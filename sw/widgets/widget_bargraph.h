#if !defined(WIDGET_BARGRAPH_H)
#define WIDGET_BARGRAPH_H

#include "rotary_encoder.h"
#include "ui_mvc.h"
#include "widget_bar.h"
#include "widget_simple_bargraph.h"
#include "controlled_value.h"
#include <array>
#include <vector>

enum class ControlMode
{
    BAND_CONTROL,
    THRESHOLD_CONTROL
};

enum class StatusFlagMode
{
    BORDER_FLAG,
    SQUARE_FLAG
};

typedef struct config_bargraph_widget
{
    // config UI_Widget
    uint8_t bargraph_anchor_x;
    uint8_t bargraph_anchor_y;
    size_t bargraph_width;
    size_t bargraph_height;
    bool with_border{false};
    bool with_status_flag{true};
    StatusFlagMode status_flag_mode{StatusFlagMode::BORDER_FLAG};
    uint8_t bargraph_bin_number;
    uint8_t bargraph_bin_spacing{1};
    uint8_t bargraph_bin_flag_width{5};
    uint8_t border_width{1};

    // config Framebuffer
    Framebuffer_format format{Framebuffer_format::MONO_VLSB};
    config_framebuffer_text_t txt_cnf{.font = font_8x8};
} config_bargraph_widget_t;


//---------class W_HBargraph : public UI_Widget
class W_HBargraph : public AbstractWidget, public UI_ControlledObject
{
private:
    BargraphDisplayedObject *displayed_values;
    ControlMode control_mode{ControlMode::BAND_CONTROL};
    int threshold_increment;
    void increment_threshold();
    void decrement_threshold();

    bool with_status_flag;
    StatusFlagMode status_flag_mode;
    uint8_t bargraph_bin_number;
    uint8_t bargraph_bin_spacing;
    uint8_t bargraph_bin_flag_width;

    uint8_t bar_height;

    uint8_t px_max;
    uint8_t px_min;
    uint8_t bar_width;
    float level_coef;
    int level_offset;

    uint8_t convert_level_value_to_px(int level);
    void draw();
    void draw_bar(uint8_t bin_number);
    void draw_status_flag(uint8_t bin_number);
    void draw_threshold();
    void set_value_clipped(int new_value);

public:
    W_HBargraph(UI_DisplayDevice *display_screen, BargraphDisplayedObject *displayed_values, config_bargraph_widget_t cnf_bargraph);
    ~W_HBargraph();

    int threshold;
    uint8_t current_active_index;
    void process_control_event(ControlEvent event);
};

#endif // WIDGET_BARGRAPH_H
