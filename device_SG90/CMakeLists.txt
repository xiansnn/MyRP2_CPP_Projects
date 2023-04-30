include_directories(${PROJECT_SOURCE_DIR})
add_executable(test_sg90    test_sg90.cpp    )
target_link_libraries(test_sg90 pico_stdlib hardware_pwm hw_pwm)
pico_add_extra_outputs(test_sg90)
