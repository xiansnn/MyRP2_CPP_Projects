#include "mpu6050.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <math.h>
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

MPUData_t measures{};
RawData_t raw{};
bool data_ready = false;

void print_measures()
{
    printf("AccX = %.2f\tY = %.2f\tZ = %.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\tGyroX = %.2f\tY = %.2f\tZ = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
};

void print_raw_data()
{
    printf("Acc [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw.g_x, raw.g_y, raw.g_z);
    printf("\t\t\tGyro [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw.gyro_x, raw.gyro_y, raw.gyro_z);
    printf("\n\n");
};
void process_data_from_register(MPU6050 mpu)
{
    mpu.read_MPU_all_measure_from_registers(&measures);
    print_measures();
};
void process_data_from_FIFO(MPU6050 mpu)
{
    mpu.read_MPU_g_accel_measures_from_FIFO(&measures);
    print_measures();
};

void process_raw_data_from_FIFO(MPU6050 mpu) // TODO check FIFO data reading.... seems not working well
{
    mpu.read_FIFO_g_accel_raw_data(&raw);
    print_raw_data();
};

void gpio_callback(uint gpio, uint32_t events)
{
    pr_D1.hi();
    mpu.read_MPU_all_measure_from_registers(&measures);
    // mpu.read_MPU_all_measure_from_FIFO(&measures); // TODO check FIFO data reading.... seems not working well
    // mpu.read_MPU_g_accel_measures_from_FIFO(&measures); // TODO check FIFO data reading.... seems not working well
    data_ready = true;
    pr_D1.lo();
}

int main()
{
    stdio_init_all();
    printf("temperature : %.2f\n", mpu.read_MPU_temperature());

    gpio_set_irq_enabled_with_callback(MPU_INT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true)
    {
        pr_D4.hi();
        if (data_ready)
        {
            pr_D5.hi();
            print_measures();
            data_ready = false;
            pr_D5.lo();
        }
        pr_D4.lo();
        sleep_ms(1000);
    }
    return 0;
}