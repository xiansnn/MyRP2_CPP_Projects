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

void print_measures(MPUData_t measures)
{
    printf("AccX = %+.2f\tY = %+.2f\tZ = %+.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %+.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\tGyroX = %+.2f\tY = %+.2f\tZ = %+.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
}

int main()
{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(i2c_cfg);
    MPU6050 mpu = MPU6050(&master, mpu_cfg);
    float t = mpu.get_MPU_temperature();
    printf("temperature : %.2f\n", t);
    while (true)
    {
        if (mpu.is_data_ready())
        {
            MPUData_t measures = mpu.get_measures();
            print_measures(measures);
        }
        sleep_ms(1000);
    }
    return 0;
}
