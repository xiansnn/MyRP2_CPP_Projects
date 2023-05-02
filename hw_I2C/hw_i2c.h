#if !defined(HW_I2C_H)
#define HW_I2C_H

#include "i2c_bus_config.h"

#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"
#include <set>

class hw_I2C_master
{
private:
    i2c_inst_t *i2c;

public:
    hw_I2C_master(i2c_inst_t *i2c, uint sda, uint scl, uint baud_rate);
    int single_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t mem_value);
    int burst_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t *src, uint8_t len);
    int single_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest);
    int burst_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest, uint8_t len);
    std::set<uint8_t> bus_scan();
    void show_bus_map();
};

typedef struct
{
    uint8_t mem[I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE]{};
    uint8_t mem_address = I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE-1;//init: the slave points to its last memory address
    bool mem_address_written = false;
} context_t;

class hw_I2C_slave
{
private:
    i2c_inst_t *i2c;

public:
    context_t context;
    hw_I2C_slave(i2c_inst_t *i2c, uint sda, uint scl, uint bad_rate,
                 uint8_t slave_address, i2c_slave_handler_t handler);
    void slave_isr(i2c_slave_event_t event);
};

#endif // HW_I2C_H
