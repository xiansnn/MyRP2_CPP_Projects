#include <stdio.h>
#include <string.h>
#include "hw_i2c.h"
#include "probe.h"

#define BAUD_RATE 50000
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


// The slave implements a 256 byte memory. To write a series of bytes, the master first
// writes the memory address, followed by the data. The address is automatically incremented
// for each byte transferred, looping back to 0 upon reaching the end. Reading is done
// sequentially from the current memory address.
#define MAX_SLAVE_MEMORY_SIZE 256
static struct
{
    uint8_t mem[MAX_SLAVE_MEMORY_SIZE]{};
    uint8_t mem_address;
    bool mem_address_written;
} context;

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D4.hi();
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // master has written some data
        if (!context.mem_address_written)
        {
            // writes always start with the memory address
            context.mem_address = i2c_read_byte_raw(i2c);
            context.mem_address_written = true;
        }
        else
        {
            // save into memory
            context.mem[context.mem_address] = i2c_read_byte_raw(i2c);
            context.mem_address++;
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        // load from memory
        i2c_write_byte_raw(i2c, context.mem[context.mem_address]);
        context.mem_address++;
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        context.mem_address_written = false;
        break;
    default:
        break;
    }
    pr_D4.lo();
}

int main()
{
    stdio_init_all();
    printf("test I2C exchange \n");
#define MAX_DATA_SIZE 32

    for (uint8_t mem_address = 0;; mem_address = (mem_address + MAX_DATA_SIZE) % MAX_SLAVE_MEMORY_SIZE)
    {
        uint8_t write_data[MAX_DATA_SIZE];
        char write_msg[MAX_DATA_SIZE];
        snprintf(write_msg, sizeof(write_msg), "Hello, slave@0x%02X mem[0x%02X]", SLAVE_ADDR, mem_address);
        uint8_t msg_len = strlen(write_msg);
        memcpy(write_data, write_msg, msg_len); // to convert  char[] to uint8_t[]
        // write data at mem_address
        printf("Write at 0x%02X: '%s'\n", mem_address, write_msg);
        pr_D5.hi();
        master.burst_byte_write(SLAVE_ADDR, mem_address, write_data, msg_len);
        pr_D5.lo();

        // read from mem_address
        uint8_t read_data[MAX_DATA_SIZE];
        char read_msg[MAX_DATA_SIZE];
        pr_D6.hi();
        master.burst_byte_read(SLAVE_ADDR, mem_address, read_data, MAX_DATA_SIZE);
        pr_D6.lo();
        memcpy(read_msg, read_data, msg_len);
        msg_len = strlen(read_msg);
        printf("Read %d char at 0x%02X: '%s'\n\n", msg_len, mem_address, read_msg);
        sleep_ms(1000);
    }

    return 0;
}
