add_library(ssd1306 ssd1306.cpp  )
target_link_libraries(ssd1306 pico_stdlib hardware_i2c hw_i2c framebuffer)


add_executable(test_ssd1306  test_ssd1306.cpp  )
target_link_libraries(test_ssd1306 ssd1306 pico_stdlib hardware_i2c hw_i2c framebuffer)
pico_add_extra_outputs(test_ssd1306)

# add_executable(ssd1306_i2c  ssd1306_i2c.c  )
# target_link_libraries(ssd1306_i2c pico_stdlib hardware_i2c)
# pico_add_extra_outputs(ssd1306_i2c)