add_library(tool STATIC
    tool.h
    tool.c)

target_include_directories(tool PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}  
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(tool
    pico_stdlib
    hardware_dma
    SH1106
)