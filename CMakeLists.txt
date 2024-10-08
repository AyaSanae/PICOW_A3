# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/aya/PICO/pico-sdk")
set(CMAKE_BUILD_TYPE Release)
if(CMAKE_BUILD_TYPE MATCHES Release)
  add_compile_options(-O3)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif()
set(CMAKE_SYSROOT "/home/aya/PICO/toolchains/arm-none-eabi")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(PICOW_A3 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(PICOW_A3 PICOW_A3.c )

pico_set_program_name(PICOW_A3 "PICOW_A3")
pico_set_program_version(PICOW_A3 "0.1")

pico_enable_stdio_uart(PICOW_A3 0)
pico_enable_stdio_usb(PICOW_A3 1)

add_subdirectory(SH1106)
add_subdirectory(render)
add_subdirectory(tool)
add_subdirectory(bt)
# Add the standard library to the build
target_link_libraries(PICOW_A3
    pico_stdlib
    hardware_i2c
    hardware_clocks
    hardware_dma)

# Add the standard include files to the build
target_include_directories(PICOW_A3 PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
  ${CMAKE_SOURCE_DIR}/include
  resource_page 
  SH1106
  tool
  bt
)

# Add any user requested libraries
target_link_libraries(PICOW_A3 
        pico_cyw43_arch_none
        pico_multicore
        resource_page
        pico_sync
        pico_util
        SH1106
        tool
        bt
        )

pico_add_extra_outputs(PICOW_A3)

