#include "mpu6050.h"
#include "config_MPU6050.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
#include <math.h>

int main()
{
    stdio_init_all();
    MPU6050 mpu = MPU6050();
    sleep_ms(100);
    while (true)
    {
        uint16_t count = mpu.read_FIFO_count();
        if (count > 0)
        {
            MPUData_t measures = mpu.read_MPU_measure_from_registers();
            printf("register Acc. X = %.2f, Y = %.2f, Z = %.2f", measures.g_x, measures.g_y, measures.g_z);
            float g = pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2);
            printf("\tvecteur G: %.3f", g);
            printf("\tGyro. X = %.2f Y = %.2f, Z = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
            printf("\t\tTemp. = %.2f\n", measures.temp_out);
            
            measures = mpu.read_MPU_measure_from_FIFO();
            printf("fifo Acc. X = %.2f, Y = %.2f, Z = %.2f", measures.g_x, measures.g_y, measures.g_z);
            g = pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2);
            printf("\tvecteur G: %.3f", g);
            printf("\tGyro. X = %.2f, Y = %.2f, Z = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
            printf("\t\tTemp. = %.2f\n", measures.temp_out);
            printf("\n\n");
            
        } 
        sleep_ms(5);
    }

    return 0;
}
