#include <stdio.h>
#include <string.h>
#include "hw_i2c.h"
#include "probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

hw_I2C_master master = hw_I2C_master(I2C_MASTER);
hw_I2C_slave slave = hw_I2C_slave(I2C_SLAVE, i2c_slave_handler);

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
    std::set<uint8_t> slave_set = master.bus_scan();
    for (auto &&i : slave_set)
    {
        printf("0x%2x   ", i);
    }
    printf("\n");
    master.show_bus_map();

}