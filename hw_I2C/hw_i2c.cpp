/**
 * @file hw_i2c.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief this file provides an abstract view of the I2C hardware instances. 
 * It must be considered as a set of helper to the official pico-sdk.
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hw_i2c.h"
#include <cstring>
#include <stdio.h>

/**
 * @brief Construct a new hw I2C master::hw I2C master object
 * 
 * @param i2c The I2C hardware instance. Either i2c0 or i2c1.
 * @param sda the associated SDA pin
 * @param scl the associated SCL pin
 * @param baud_rate the requested transmission speed
 */
hw_I2C_master::hw_I2C_master(i2c_inst_t *i2c, uint sda, uint scl, uint baud_rate)
{
    this->i2c = i2c;

    // As suggested by RP2040 data sheet
    gpio_init(sda);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_slew_rate(sda, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(sda, true);

    gpio_init(scl);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);
    gpio_set_slew_rate(scl, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(scl, true);

    i2c_init(i2c, baud_rate);
}

int hw_I2C_master::single_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t mem_value)
{
    uint8_t write_buf[] = {slave_mem_addr, mem_value};
    int nb = i2c_write_blocking(this->i2c, slave_address, write_buf, 2, false);
    return nb;
}

/**
 * @brief Helper to blocking write a block of data starting at a slave memory address
 * 
 * @param slave_address the slave address
 * @param slave_mem_addr the slave memory 
 * @param src the address of the block of data
 * @param len the size of the block of data
 * @return int Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged, no device present.
 */
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

/**
 * @brief A utility that scan the I2C bus and return the set of answering devices
 * 
 * @return std::set<uint8_t> the set of responding addresses
 */
std::set<uint8_t> hw_I2C_master::bus_scan()
{
    std::set<uint8_t> device_address_set;
    int nb;
    uint8_t rxdata;
    for (uint8_t addr = 0x08; addr < 0x78; addr++)
    {
        nb = i2c_read_blocking(this->i2c,addr, &rxdata,1,false);
        if (nb > 0)
            device_address_set.insert(addr);
    }
    return device_address_set;
}

/**
 * @brief a utility that provides a map of responding devices
 * 
 */
void hw_I2C_master::show_bus_map()
{
    printf("Connected slave's map \n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    std::set<uint8_t> device_address_set = this->bus_scan();
    for (size_t i = 0; i < 0x80; i++)
    {
        if (i % 16 == 0)
            printf("%02x ", i); // start of lines labels
        printf(device_address_set.contains(i) ? "@" : ((i < 0x08 || i > 0x78) ? " " : "."));
        printf(i % 16 == 15 ? "\n" : "  "); // columns interspace and end of lines
    }
    printf("done\n");
}

/**
 * @brief this is the actual Interrupt Service Routine executed by the slave after each received data
 * 
 * @param event the type of data/command received
 */
void hw_I2C_slave::slave_isr(i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // master has sent a Write command ...
        if (!this->context.mem_address_written)
        {
            // ... for the  first time. This gives the memory start address
            this->context.mem_address = i2c_read_byte_raw(this->i2c);
            this->context.mem_address_written = true; // to remind a write was received
        }
        else
        {
            // ... not the firrst time. This means the slave must copy the received data in its memory
            this->context.mem[context.mem_address] = i2c_read_byte_raw(this->i2c);
            this->context.mem_address++; // prepare for the next received data
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        // the starting address was given by the first Write command
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

/**
 * @brief Construct a new hw I2C slave::hw I2C slave object
 * 
 * @param i2c The I2C hardware instance. Either i2c0 or i2c1.
 * @param sda the associated SDA pin
 * @param scl the associated SCL pin
 * @param baud_rate the requested transmission speed
 * @param slave_address the slave address to respond with
 * @param handler the IRQ handler. NOTICE: This handler is the one given to NVIC IRQ map.
 * It seems that it must be a static function defined in the main code.
 */
hw_I2C_slave::hw_I2C_slave(i2c_inst_t *i2c, uint sda, uint scl, uint baud_rate,
                           uint8_t slave_address, i2c_slave_handler_t handler)
{
    this->i2c = i2c;

    // As suggested by RP2040 data sheet
    gpio_init(sda);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_slew_rate(sda, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(sda, true);

    gpio_init(scl);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);
    gpio_set_slew_rate(scl, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(scl, true);

    i2c_init(i2c, baud_rate);

    i2c_slave_init(i2c, slave_address, handler);
}
