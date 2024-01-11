add_executable(test_gpio_irq test_gpio_irq.cpp )

# pull in common dependencies
target_link_libraries(test_gpio_irq pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(test_gpio_irq)

# add url via pico_set_program_url
# example_auto_set_url(hello_gpio_irq)
