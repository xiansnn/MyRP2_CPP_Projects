add_library(rotary_encoder rotary_encoder.cpp)
target_link_libraries(rotary_encoder pico_stdlib controlled_value ui_mvc )


add_executable(test_ky040 test_ky_040.cpp)
target_link_libraries(test_ky040  controlled_value pico_stdlib  rotary_encoder debug_probe switch_button print_console focus_manager)
pico_add_extra_outputs(test_ky040)
