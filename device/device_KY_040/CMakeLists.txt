add_library(ky_040 ky_040.cpp)
target_link_libraries(ky_040 pico_stdlib hardware_adc)


add_executable(test_ky_040 test_ky_040.cpp)
target_link_libraries(test_ky_040  pico_stdlib  hardware_adc  ky_040)
pico_add_extra_outputs(test_ky_040)
