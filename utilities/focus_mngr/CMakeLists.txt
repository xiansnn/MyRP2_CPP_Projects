add_library( focus_manager focus_manager.cpp)
target_link_libraries(focus_manager ui_mvc switch_button ui_focus_manager)


# add_executable(test_controlled_value test_controlled_value.cpp)
# target_link_libraries(test_controlled_value controlled_value pico_stdlib debug_probe)
# pico_add_extra_outputs(test_controlled_value)