#if !defined(WIDGETS_H)
#define WIDGETS_H

#include "framebuffer.h"

typedef struct bar_widget_config
{
    int level_max{10};
    int level_min{0};
    size_t width{128};
    size_t height{8};
    bool draw_border{true};
    bool draw_value {false};
    const unsigned char *font{nullptr};
} bar_widget_config_t;

class Bar : public Framebuffer
{
private:
    size_t value_max_width;
    uint8_t px_max;
    uint8_t px_min;
    float level_coef;
    int level_offset;

    uint8_t convert_level_to_px(int level);

    

public:
    Bar(bar_widget_config_t bar_config);
    ~Bar();

    bar_widget_config_t config{};
    int level{0};
    uint8_t px;

    void reset_px();
    void increment_level();
    void decrement_level();
    void draw();
};

#endif // WIDGETS_H
