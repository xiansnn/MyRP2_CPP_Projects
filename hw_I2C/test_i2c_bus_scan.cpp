#include <stdio.h>
#include <string.h>
#include "hw_i2c.h"
#include "probe.h"

#define BAUD_RATE 100 * 000
#define I2C_MASTER i2c0
#define I2C_MASTER_SDA_PIN 8
#define I2C_MASTER_SCL_PIN 9
#define I2C_SLAVE i2c1
#define I2C_SLAVE_SDA_PIN 6
#define I2C_SLAVE_SCL_PIN 7
#define SLAVE_ADDR 0x65

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

hw_I2C_master master = hw_I2C_master(I2C_MASTER, I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN, BAUD_RATE);
hw_I2C_slave slave = hw_I2C_slave(I2C_SLAVE, I2C_SLAVE_SDA_PIN, I2C_SLAVE_SCL_PIN, BAUD_RATE,
                                  SLAVE_ADDR, i2c_slave_handler);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D4.hi();
    slave.slave_isr(event);
    pr_D4.lo();
}

int main()
{
    stdio_init_all();
    printf("test I2C bus scan : ");
    std::vector<uint8_t> slave_list = master.bus_scan();
    for (auto &&i : slave_list) printf("0x%2x   ", i);
    printf("\n");
    printf("done");
}