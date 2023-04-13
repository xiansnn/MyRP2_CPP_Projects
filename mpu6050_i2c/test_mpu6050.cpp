#include "mpu6050.h"
#include "config_MPU6050.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
#include <math.h>

void process_data(MPU6050 mpu){
    MPUData_t measures{};
    mpu.read_MPU_g_accel_measures_from_FIFO(&measures);
    printf("fifo AccX = %.2f\tY = %.2f\tZ = %.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %.3f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\tGyroX = %.2f\tY = %.2f\tZ = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");

};
void process_raw_data(MPU6050 mpu){
    RawData_t raw{};
    mpu.read_FIFO_g_accel_raw_data(&raw);
    printf("fifo AccX = %d\tY = %d\tZ = %d", raw.g_x, raw.g_y, raw.g_z);
    // printf("\tvecteur G: %d", sqrt(pow(raw.g_x, 2) + pow(raw.g_y, 2) + pow(raw.g_z, 2)));
    printf("\tGyroX = %d\tY = %d\tZ = %d", raw.gyro_x, raw.gyro_y, raw.gyro_z);
    printf("\n\n");
};

int main()
{
    stdio_init_all();
    MPU6050 mpu = MPU6050();
    // measures.temp_out = mpu.read_MPU_temperature();
    // printf("\t\tTemp. = %.2f\n", measures.temp_out);
    // sleep_ms(100);
    while (true)
    {
        while (mpu.read_FIFO_count() > 0)
        {
            // process_raw_data(mpu);
            process_data(mpu);     
        }
        sleep_ms(5);
    }
    return 0;
}
