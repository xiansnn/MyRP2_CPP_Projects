#if !defined(HW_I2C_H)
#define HW_I2C_H

#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"
#include <vector>

#define MAX_SLAVE_MEMORY_SIZE 256

class hw_I2C_master
{
private:
    i2c_inst_t *i2c;
    std::vector<uint8_t> slave_list;

public:
    hw_I2C_master(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate);
    int single_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t mem_value);
    int burst_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t *src, uint8_t len);
    int single_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest);
    int burst_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest, uint8_t len);
    std::vector<uint8_t> bus_scan();

};

typedef struct
{
    uint8_t mem[MAX_SLAVE_MEMORY_SIZE]{};
    uint8_t mem_address;
    bool mem_address_written;
} context_t;

class hw_I2C_slave
{
private:
    i2c_inst_t *i2c;
    context_t context;

public:
    hw_I2C_slave(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate,
                 uint8_t slave_address, i2c_slave_handler_t handler);
    void slave_isr(i2c_slave_event_t event);
};

#endif // HW_I2C_H
