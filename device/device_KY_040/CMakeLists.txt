add_library(ky040 ky_040.cpp)
target_link_libraries(ky040 pico_stdlib hardware_adc)


add_executable(test_ky040 test_ky_040.cpp)
target_link_libraries(test_ky040  controlled_value pico_stdlib  hardware_adc  ky040 debug_probe switch_button)
pico_add_extra_outputs(test_ky040)
