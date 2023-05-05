/**
 * @file test_i2c_bus_scan.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <string.h>
#include "hw_i2c.h"
#include "probe.h"



#define I2C_0_BAUD_RATE I2C_STANDARD_MODE
#define I2C_0_SDA_PIN 8 // choice in [0, (4), 8, 12, 16, 20]   (default)
#define I2C_0_SCL_PIN 9 // choice in [1, (5), 9, 13, 17, 21]   (default)
#define I2C_0_SLAVE_ADDR 0x55 // in case I2C0 is slave (default 0x55)
#define I2C_0_SLAVE_MAX_MEMORY_SIZE 256 // in case I2C0 is slave

#define I2C_1_BAUD_RATE I2C_STANDARD_MODE
#define I2C_1_SDA_PIN 6 // choice in [2, 6, 10, 14, 18, 26]
#define I2C_1_SCL_PIN 7 // choice in [3, 7, 11, 15, 19, 27]
#define I2C_1_SLAVE_ADDR 0x15 // in case I2C0 is slave (default 0x55)
#define I2C_1_SLAVE_MAX_MEMORY_SIZE 256 // in case I2C0 is slave



Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

hw_I2C_master master = hw_I2C_master(i2c0, I2C_0_SDA_PIN, I2C_0_SCL_PIN, I2C_0_BAUD_RATE);
hw_I2C_slave slave = hw_I2C_slave(i2c1, I2C_1_SDA_PIN, I2C_1_SCL_PIN, I2C_1_BAUD_RATE,
                                  I2C_1_SLAVE_ADDR, i2c_slave_handler);                          


/**
 * @brief the actual I2C IRQ handler given to the RP2040 hardware I2C instance
 * 
 * @param i2c Either i2c0 or i2c1
 * @param event the event that gives the type of received data/command
 */
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D4.hi();
    slave.slave_isr(event);
    pr_D4.lo();
}

int main()
{
    stdio_init_all();
    slave.context.mem[I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE-1]=I2C_1_SLAVE_ADDR;//just for the fun:the slave answer its address located in its last memory
    printf("test I2C bus scan : ");
    std::set<uint8_t> slave_set = master.bus_scan();
    for (auto &&i : slave_set)
        printf("0x%2x   ", i);
    printf("\n");
    master.show_bus_map();

}