include_directories(${PROJECT_SOURCE_DIR})

add_executable(mpu6050
        mpu6050.cpp
        test_mpu6050.cpp
        )

target_link_libraries(mpu6050 pico_stdlib hardware_i2c)

pico_add_extra_outputs(mpu6050)
