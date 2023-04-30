#include "hw_i2c.h"
#include <cstring>

hw_I2C_master::hw_I2C_master(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate)
{
    this->i2c = i2c;
    gpio_init(sda_pin);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);

    gpio_init(scl_pin);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(scl_pin);

    i2c_init(this->i2c, baud_rate);
}

int hw_I2C_master::single_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t mem_value)
{
    uint8_t write_buf[] = {slave_mem_addr, mem_value};
    int nb = i2c_write_blocking(this->i2c, slave_address, write_buf, 2, false);
    return nb;
}

int hw_I2C_master::burst_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *src, uint8_t len)
{
    uint8_t write_buf[len + 1] = {slave_mem_addr};
    memcpy(write_buf + 1, src, len);
    int nb = i2c_write_blocking(this->i2c, slave_address, write_buf, len + 1, false);
    return nb;
}

int hw_I2C_master::single_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *dest)
{
    uint8_t cmd_buf[]{slave_mem_addr};
    i2c_write_blocking(this->i2c, slave_address, cmd_buf, 1, true);
    int nb = i2c_read_blocking(this->i2c, slave_address, dest, 1, false);
    return nb;
}

int hw_I2C_master::burst_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *dest, uint8_t len)
{
    uint8_t cmd_buf[]{slave_mem_addr};
    i2c_write_blocking(this->i2c, slave_address, cmd_buf, 1, true);
    int nb = i2c_read_blocking(this->i2c, slave_address, dest, len, false);
    return nb;
}

std::vector<uint8_t> hw_I2C_master::bus_scan()
{
    this->slave_list.clear();

    for (uint8_t addr = 0; addr < (1 << 7); ++addr)
    {
        int nb;
        uint8_t rxdata;
        if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78)
            nb = PICO_ERROR_GENERIC;
        else
        {
            nb = this->single_byte_read(addr, 0x00, &rxdata);
            if (nb > 0)
            {
                this->slave_list.push_back(addr);
            }
        }
    }

    return this->slave_list;
}

void hw_I2C_slave::slave_isr(i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // master has written some data
        if (!this->context.mem_address_written)
        {
            // writes always start with the memory address
            this->context.mem_address = i2c_read_byte_raw(this->i2c);
            this->context.mem_address_written = true;
        }
        else
        {
            // save into memory
            this->context.mem[context.mem_address] = i2c_read_byte_raw(this->i2c);
            this->context.mem_address++;
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        // load from memory
        i2c_write_byte_raw(this->i2c, this->context.mem[context.mem_address]);
        this->context.mem_address++;
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        this->context.mem_address_written = false;
        break;
    default:
        break;
    }
}

hw_I2C_slave::hw_I2C_slave(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baud_rate,
                           uint8_t slave_address, i2c_slave_handler_t handler)
{
    this->i2c = i2c;
    gpio_init(sda_pin);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);

    gpio_init(scl_pin);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(scl_pin);

    i2c_init(i2c, baud_rate);

    i2c_slave_init(i2c, slave_address, handler); // configure I2C0 for slave mode
}
