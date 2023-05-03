
add_library(mpu6050 mpu6050.cpp)
target_link_libraries(mpu6050 pico_stdlib hardware_i2c)

add_executable(test_mpu6050   test_mpu6050.cpp   )
target_link_libraries(test_mpu6050 pico_stdlib mpu6050 hardware_i2c )
pico_add_extra_outputs(test_mpu6050)
