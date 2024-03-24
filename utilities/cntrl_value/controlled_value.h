#if !defined(CNTRL_VALUE_H)
#define CNTRL_VALUE_H

#include "ui_mvc.h"

class ControlledValue : public UI_ControlledObject
{
private:
public:
    ControlledValue(uint8_t id, int min_value, int max_value, int increment = 1, bool wrap = false);
    ~ControlledValue();

    void process_control_event(ControlEvent event);
    void set_value_clipped(int new_value);
};

class SingleControlledValue : public AbstractModelObject
{
private:

protected:
    int value;
    int min_value;
    int max_value;
    int increment{1};
    bool wrap;

public:
    SingleControlledValue(int min_value = 0, int max_value = 10, bool wrap = false, int increment = 1);
    ~SingleControlledValue();

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


#endif // CNTRL_VALUE_H
