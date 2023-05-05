#include <stdio.h>
#include <string.h>
#include "hw_i2c.h"
#include "probe.h"

#define I2C_0_BAUD_RATE I2C_STANDARD_MODE
#define I2C_0_SDA_PIN 8                 // choice in [0, (4), 8, 12, 16, 20]   (default)
#define I2C_0_SCL_PIN 9                 // choice in [1, (5), 9, 13, 17, 21]   (default)
#define I2C_0_SLAVE_ADDR 0x55           // in case I2C0 is slave (default 0x55)
#define I2C_0_SLAVE_MAX_MEMORY_SIZE 256 // in case I2C0 is slave

#define I2C_1_BAUD_RATE I2C_STANDARD_MODE
#define I2C_1_SDA_PIN 6                 // choice in [2, 6, 10, 14, 18, 26]
#define I2C_1_SCL_PIN 7                 // choice in [3, 7, 11, 15, 19, 27]
#define I2C_1_SLAVE_ADDR 0x15           // in case I2C0 is slave (default 0x55)
#define I2C_1_SLAVE_MAX_MEMORY_SIZE 256 // in case I2C0 is slave

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

hw_I2C_master master = hw_I2C_master(i2c0, I2C_0_SDA_PIN, I2C_0_SCL_PIN, I2C_0_BAUD_RATE);
hw_I2C_slave slave = hw_I2C_slave(i2c1, I2C_1_SDA_PIN, I2C_1_SCL_PIN, I2C_1_BAUD_RATE,
                                  I2C_1_SLAVE_ADDR, i2c_slave_handler);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D4.hi();
    slave.slave_isr(event);
    pr_D4.lo();
}

#define MAX_DATA_SIZE 32
int main()
{
    stdio_init_all();
    printf("test I2C exchange \n");
    if (not master.device_is_connected(I2C_1_SLAVE_ADDR))
    {
        printf("no device at 0x%02x\n", I2C_1_SLAVE_ADDR);
    }
    else
    {
        for (uint8_t mem_address = 0;; mem_address = (mem_address + MAX_DATA_SIZE) % I2C_1_SLAVE_MAX_MEMORY_SIZE)
        {
            uint8_t write_data[MAX_DATA_SIZE];
            char write_msg[MAX_DATA_SIZE];
            snprintf(write_msg, sizeof(write_msg), "Hello, slave@0x%02X mem[0x%02X]", I2C_1_SLAVE_ADDR, mem_address);
            uint8_t msg_len = strlen(write_msg);
            memcpy(write_data, write_msg, msg_len); // to convert  char[] to uint8_t[]
            // write data at mem_address
            printf("Write at 0x%02X: '%s'\n", mem_address, write_msg);
            pr_D5.hi();
            master.burst_byte_write(I2C_1_SLAVE_ADDR, mem_address, write_data, msg_len);
            pr_D5.lo();

            // read from mem_address
            uint8_t read_data[MAX_DATA_SIZE];
            char read_msg[MAX_DATA_SIZE];
            pr_D6.hi();
            master.burst_byte_read(I2C_1_SLAVE_ADDR, mem_address, read_data, MAX_DATA_SIZE);
            pr_D6.lo();
            memcpy(read_msg, read_data, MAX_DATA_SIZE);
            msg_len = strlen(read_msg);
            printf("Read %d char at 0x%02X: '%s'\n\n", msg_len, mem_address, read_msg);
            sleep_ms(1000);
        }
    }
    return 0;
}
