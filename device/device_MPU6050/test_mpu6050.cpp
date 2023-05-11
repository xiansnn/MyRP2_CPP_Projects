#include "mpu6050.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
#include <math.h>
#include "probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

config_master_i2c_t i2c_cfg{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_STANDARD_MODE};

config_MPU6050_t mpu_cfg{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 5};

void process_data_from_register(MPU6050 mpu)
{
    pr_D5.hi();
    MPUData_t measures{};
    mpu.read_MPU_all_measure_from_registers(&measures);
    pr_D5.lo();
    pr_D4.hi();
    printf("reg. accX = %.2f\taccY = %.2f\taccZ = %.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\t\tgyroX = %.2f\tgyroY = %.2f\tgyroZ = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
    pr_D4.lo();
};
void process_data_from_FIFO(MPU6050 mpu)
{
    MPUData_t measures{};
    mpu.read_MPU_g_accel_measures_from_FIFO(&measures);
    printf("fifo AccX = %.2f\tY = %.2f\tZ = %.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\tGyroX = %.2f\tY = %.2f\tZ = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
};

void process_raw_data_from_FIFO(MPU6050 mpu) //TODO check FIFO data reading.... seems not working well
{
    RawData_t raw{};
    mpu.read_FIFO_g_accel_raw_data(&raw);
    printf(" FIFO Acc [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw.g_x, raw.g_y, raw.g_z);
    printf("\t\t\tGyro [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw.gyro_x, raw.gyro_y, raw.gyro_z);
    printf("\n\n");
};

int main()
{
    stdio_init_all();
    // create I2C bus hw peripheral
    hw_I2C_master master = hw_I2C_master(i2c_cfg);
    MPU6050 mpu = MPU6050(&master, mpu_cfg);
    float t = mpu.read_MPU_temperature();
    printf("temperature : %.2f\n", t);
    while (true)
    {
        if (mpu.is_data_ready())
        {
            // process_raw_data_from_FIFO(mpu);
            process_data_from_register(mpu);
            // process_data_from_FIFO(mpu);
        }
        sleep_ms(5);
    }
    return 0;
}
