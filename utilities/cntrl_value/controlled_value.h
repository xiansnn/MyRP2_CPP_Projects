#if !defined(CNTRL_VALUE_H)
#define CNTRL_VALUE_H

class ControlledValue
{
private:
    int min_value;
    int max_value;
    int value{0};
    int increment{1};


public:
    ControlledValue(int min_value, int max_value, int increment = 1);
    ~ControlledValue();

    bool has_changed{false};

    int get_value();
    void set_value(int new_value);
    void reset_value();
    void increment_value();
    void decrement_value();
    void clear_change_flag();
};

#endif // CNTRL_VALUE_H
