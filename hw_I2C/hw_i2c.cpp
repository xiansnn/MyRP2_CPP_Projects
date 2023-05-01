#include "hw_i2c.h"
#include <cstring>
#include <stdio.h>

hw_I2C_master::hw_I2C_master(i2c_inst_t *i2c)
{
    this->i2c = i2c;
    gpio_init(I2C_MASTER_SDA_PIN);
    gpio_set_function(I2C_MASTER_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_MASTER_SDA_PIN);

    gpio_init(I2C_MASTER_SCL_PIN);
    gpio_set_function(I2C_MASTER_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_MASTER_SCL_PIN);

    i2c_init(I2C_MASTER, I2C_MASTER_BAUD_RATE);
}

int hw_I2C_master::single_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t mem_value)
{
    uint8_t write_buf[] = {slave_mem_addr, mem_value};
    int nb = i2c_write_blocking(I2C_MASTER, slave_address, write_buf, 2, false);
    return nb;
}

int hw_I2C_master::burst_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *src, uint8_t len)
{
    uint8_t write_buf[len + 1] = {slave_mem_addr};
    memcpy(write_buf + 1, src, len);
    int nb = i2c_write_blocking(I2C_MASTER, slave_address, write_buf, len + 1, false);
    return nb;
}

int hw_I2C_master::single_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *dest)
{
    uint8_t cmd_buf[]{slave_mem_addr};
    i2c_write_blocking(I2C_MASTER, slave_address, cmd_buf, 1, true);
    int nb = i2c_read_blocking(I2C_MASTER, slave_address, dest, 1, false);
    return nb;
}

int hw_I2C_master::burst_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *dest, uint8_t len)
{
    uint8_t cmd_buf[]{slave_mem_addr};
    i2c_write_blocking(I2C_MASTER, slave_address, cmd_buf, 1, true);
    int nb = i2c_read_blocking(I2C_MASTER, slave_address, dest, len, false);
    return nb;
}

std::set<uint8_t> hw_I2C_master::bus_scan()
{
    std::set<uint8_t> slave_set;
    int nb;
    uint8_t rxdata;
    for (uint8_t addr = 0x08; addr < 0x78; addr++)
    {
        nb = this->single_byte_read(addr, 0x00, &rxdata);
        if (nb > 0)
            slave_set.insert(addr);
    }
    return slave_set;
}

void hw_I2C_master::show_bus_map()
{
    printf("Connected slave's map \n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    std::set<uint8_t> slave_set = this->bus_scan();
    for (size_t i = 0; i < 0x80; i++)
    {
        if (i % 16 == 0)
            printf("%02x ", i); // start of lines labels
        printf(slave_set.contains(i) ? "@" : ((i < 0x08 || i > 0x78) ? " " : "."));
        printf(i % 16 == 15 ? "\n" : "  "); // columns interspace and end of lines
    }
    printf("done\n");
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


// void hw_I2C_slave::i2c_slave_handler(i2c_inst_t * i2c, i2c_slave_event_t event)
// {
//     switch (event)
//     {
//     case I2C_SLAVE_RECEIVE: // master has written some data
//         if (!hw_I2C_slave::memory.mem_address_written)
//         {
//             // writes always start with the memory address
//             hw_I2C_slave::memory.mem_address = i2c_read_byte_raw(i2c1);
//             hw_I2C_slave::memory.mem_address_written = true;
//         }
//         else
//         {
//             // save into memory
//             hw_I2C_slave::memory.mem[hw_I2C_slave::memory.mem_address] = i2c_read_byte_raw(i2c1);
//             hw_I2C_slave::memory.mem_address++;
//         }
//         break;
//     case I2C_SLAVE_REQUEST: // master is requesting data
//         // load from memory
//         i2c_write_byte_raw(I2C_SLAVE, hw_I2C_slave::memory.mem[hw_I2C_slave::memory.mem_address]);
//         hw_I2C_slave::memory.mem_address++;
//         break;
//     case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
//         hw_I2C_slave::memory.mem_address_written = false;
//         break;
//     default:
//         break;
//     }

// }

hw_I2C_slave::hw_I2C_slave(i2c_inst_t *i2c ,i2c_slave_handler_t handler)
{
    this->i2c = i2c;

    gpio_init(I2C_SLAVE_SDA_PIN);
    gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SDA_PIN);

    gpio_init(I2C_SLAVE_SCL_PIN);
    gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SCL_PIN);

    i2c_init(I2C_SLAVE, I2C_SLAVE_BAUD_RATE);

    i2c_slave_init(I2C_SLAVE, I2C_SLAVE_ADDR, handler); // configure I2C0 for slave mode
}
