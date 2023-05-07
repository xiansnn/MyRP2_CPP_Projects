

add_library(sg90 sg90.cpp)
target_link_libraries(sg90  pico_stdlib hw_pwm)


add_executable(test_sg90    test_sg90.cpp    )
target_link_libraries(test_sg90 sg90 )
pico_add_extra_outputs(test_sg90)
