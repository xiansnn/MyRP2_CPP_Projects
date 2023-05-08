add_executable(servo servo.cpp)
target_include_directories(servo PUBLIC 
        ${PROJECT_SOURCE_DIR}/device/device_MPU6050
        ${PROJECT_SOURCE_DIR}/device/device_SG90
        ${PROJECT_SOURCE_DIR}/utilities/dbg_probe)
target_link_libraries(servo PUBLIC sg90 mpu6050 debug_probe)