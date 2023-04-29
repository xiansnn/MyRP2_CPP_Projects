#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"

class hw_I2C_master
{
private:
    i2c_inst_t *i2c;

public:
    hw_I2C_master(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate);
};

class hw_I2C_slave

{
private:
    /* data */
public:
    hw_I2C_slave(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate,
                  uint8_t slave_address, i2c_slave_handler_t handler);
};
