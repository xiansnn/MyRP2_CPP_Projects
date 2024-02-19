#if !defined(CNTRL_VALUE_H)
#define CNTRL_VALUE_H

#include "ui_mvc.h"

class ControlledValue : public UI_ControlledObject
{
private:
    int value;
    int increment_val{1};
    int min_value;
    int max_value;
    bool wrap;

public:
    ControlledValue(uint8_t id, int min_value, int max_value, int increment = 1, bool wrap = false) ;
    ~ControlledValue();

    int get_value();
    int get_min_value();
    int get_max_value();
    void set_value(int new_value);
    void reset();

    void increment();
    void decrement();
    void on_push();
    void on_long_push();
    void on_short_release();
    void on_long_release();
};

#endif // CNTRL_VALUE_H
