#include "sg90.h"
#include "mpu6050.h"
#include <stdio.h>
#include "probe.h"

//-----------------------------------------------------------------------------
//                             MPU6050 I2C bus and sensor config
config_master_i2c i2c_bus_cfg{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_STANDARD_MODE
};
config_MPU6050_t sensor_config{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 5
};
//-----------------------------------------------------------------------------
//                             SG90 config
config_sg90_t motor_config{
    .command_pin = 7,
    .sync_pin = 6
};
//-----------------------------------------------------------------------------

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);


int main()
{
    stdio_init_all();
    hw_I2C_master bus_master = hw_I2C_master(i2c_bus_cfg);
    SG90 motor = SG90(motor_config);
    MPU6050 sensor = MPU6050(&bus_master,sensor_config);
    MPUData_t measures;
    float dev;

    while (true)
    {
        pr_D4.hi();
        sensor.read_MPU_all_measure_from_registers(&measures);
        pr_D4.lo();
        dev = measures.g_z;
        // printf("dev : %+000.1f \n", dev);
        int pos = motor_config.pos_max_degree * dev;
        pr_D5.hi();
        motor.set_pos(pos);
        pr_D5.lo();
        sleep_ms(20);
    }
    
    return 0;
}
