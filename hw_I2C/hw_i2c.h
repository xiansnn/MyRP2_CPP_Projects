#if !defined(HW_I2C_H)
#define HW_I2C_H

#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"

class hw_I2C_master
{
private:
    i2c_inst_t *i2c;

public:
    hw_I2C_master(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate);
    uint8_t single_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t mem_value);
    uint8_t burst_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t *src, uint8_t len);
    uint8_t single_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest);
    uint8_t burst_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest, uint8_t len);
};

class hw_I2C_slave

{
private:
    void isr();
public:
    hw_I2C_slave(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate,
                 uint8_t slave_address, i2c_slave_handler_t handler);
};

#endif // HW_I2C_H
