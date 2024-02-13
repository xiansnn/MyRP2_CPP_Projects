#if !defined(CNTRL_VALUE_H)
#define CNTRL_VALUE_H

class ControlledValue
{
private:
    int value;
    int increment{1};
    int min_value;
    int max_value;
    bool wrap;


public:
    ControlledValue(int min_value, int max_value, int increment = 1, bool wrap = false);
    ~ControlledValue();

    bool has_changed{false};

    int get_value();
    int get_min_value();
    int get_max_value();
    void set_value(int new_value);
    void reset_value();
    void increment_value();
    void decrement_value();
    void clear_change_flag();
};

#endif // CNTRL_VALUE_H
