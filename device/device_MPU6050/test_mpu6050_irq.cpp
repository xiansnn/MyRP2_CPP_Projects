#include "mpu6050.h"
// #include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include "probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D1 = Probe(1);

#define MPU_INT 6 // gpio connnected to MPU INT pin

config_master_i2c_t i2c_cfg{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_STANDARD_MODE};

config_MPU6050_t mpu_cfg{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 5};

// create I2C bus hw peripheral and MPU
hw_I2C_master master = hw_I2C_master(i2c_cfg);
MPU6050 mpu = MPU6050(&master, mpu_cfg);

bool data_ready = false;

void gpio_callback(uint gpio, uint32_t events)
{
    pr_D1.hi();
    mpu.get_measures();
    data_ready = true;
    pr_D1.lo();
}

int main()
{
    stdio_init_all();
    printf("temperature : %.2f\n", mpu.get_MPU_temperature());

    gpio_set_irq_enabled_with_callback(MPU_INT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true)
    {
        pr_D4.hi();
        if (data_ready)
        {
            pr_D5.hi();
            mpu.print_measures();
            data_ready = false;
            pr_D5.lo();
        }
        pr_D4.lo();
        sleep_ms(1000);
    }
    return 0;
}