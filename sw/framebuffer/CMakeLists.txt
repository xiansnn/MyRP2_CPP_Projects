add_library(framebuffer framebuffer.cpp)
target_link_libraries(framebuffer pico_stdlib)


add_executable(test_streams test_streams.cpp)
target_link_libraries(test_streams ssd1306 pico_stdlib hardware_i2c hw_i2c framebuffer)
pico_add_extra_outputs(test_streams)
