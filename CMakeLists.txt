cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
include_directories(/home/thesquashy/pico/pico-sdk/src/common/pico_sdk_headers)

project(pwmctl C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pwmctl
    pwmctl.c
)
pico_enable_stdio_usb(pwmctl 1)
pico_add_extra_outputs(pwmctl)
target_link_libraries(pwmctl
    pico_i2c_slave
    hardware_i2c
    pico_stdlib
    hardware_pwm
    )