
add_library(mpu6050 mpu6050.cpp)
target_link_libraries(mpu6050 pico_stdlib hardware_i2c hw_i2c )

add_executable(test_mpu6050   test_mpu6050.cpp   )
target_link_libraries(test_mpu6050 pico_stdlib mpu6050 hardware_i2c hw_i2c debug_probe)
pico_add_extra_outputs(test_mpu6050)

add_executable(test_mpu6050_irq   test_mpu6050_irq.cpp   )
target_link_libraries(test_mpu6050_irq pico_stdlib mpu6050 hardware_i2c hw_i2c debug_probe)
pico_add_extra_outputs(test_mpu6050_irq)
