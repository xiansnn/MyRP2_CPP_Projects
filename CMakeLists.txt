cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(MYRP2_CPP_PROJECTS C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

include_directories(${PROJECT_SOURCE_DIR}/utilities/dbg_probe
                    ${PROJECT_SOURCE_DIR}/utilities/cntrl_value
                    ${PROJECT_SOURCE_DIR}/utilities/focus_mngr
                    ${PROJECT_SOURCE_DIR}/sw/framebuffer
                    ${PROJECT_SOURCE_DIR}/device/device_ssd1306
                    ${PROJECT_SOURCE_DIR}/device/device_switch
                    ${PROJECT_SOURCE_DIR}/device/device_KY_040
                    ${PROJECT_SOURCE_DIR}/hw/hw_I2C
                    ${PROJECT_SOURCE_DIR}/hw/hw_PWM)

add_subdirectory(utilities)
add_subdirectory(hw)
add_subdirectory(device)
add_subdirectory(sw)
add_subdirectory(app_example)

